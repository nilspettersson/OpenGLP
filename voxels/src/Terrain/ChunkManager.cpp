#include "ChunkManager.h"
#include <thread>

ChunkManager::ChunkManager(int chunks, int chunkWidth, int chunkHeight):
	shader("res/shaders/main.shader"),
	textureAtlas("res/textures/test.png", glp::Texture::FILTER::LINEAR, 16) {

	this->chunkWidth = chunkWidth;
	this->chunkHeight = chunkHeight;
	this->chunkCount = chunks;

	this->originX = 0;
	this->originZ = 0;

	this->isRunning = true;
	this->updateChunks(0, 0);
	this->meshThread = std::thread(&ChunkManager::CreateChunkMesh, this);
}

void ChunkManager::updateChunks(int originX, int originZ) {
	//std::cout << "updateChunks " << this->chunks.size() << std::endl;
	this->originX = originX;
	this->originZ = originZ;

	this->CreateEntities();

	std::shared_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
	auto chunksToBeRemoved = std::vector<ChunkGenerator*>();
	if (lock.try_lock()) {
		int removeCount = 8;
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			float deltaX = (chunk->chunkX + originX);
			float deltaZ = (chunk->chunkZ + originZ);

			float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);

			if (distance > chunkCount) {
				chunksToBeRemoved.push_back(this->chunks.at(i->first));
				removeCount--;
			}
			if (removeCount == 0) break;
		}
		lock.unlock();
	}

	if (chunksToBeRemoved.size() > 0) {
		std::unique_lock<std::shared_mutex> lock2(chunksMutex, std::defer_lock);
		if (lock2.try_lock()) {
			for (auto chunk : chunksToBeRemoved) {
				//std::lock_guard<std::mutex> lock(chunk->chunkLock);
				std::string key = std::to_string((int)chunk->chunkX) + "|" + std::to_string((int)chunk->chunkZ);
				delete chunk;
				this->chunks.at(key) = nullptr;
				this->chunks.erase(key);
			}
			lock2.unlock();
		}
	}
}

void ChunkManager::generateChunks() {
	bool chunkCreated = false;
	for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
		for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float detail = 1;
			float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);
			float distance2 = glm::sqrt(x * x + y * y);

			if (distance > chunkCount) continue;

			if (distance > 46) {
				detail /= 16;
			}
			else if (distance > 24) {
				detail /= 8;
			}
			else if (distance > 12) {
				detail /= 4;
			}
			else if (distance > 6) {
				detail /= 2;
			}
			//detail = 1;

			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			std::shared_lock<std::shared_mutex> lock(chunksMutex);
			if (this->chunks.find(key) == this->chunks.end()) {
				chunkCreated = true;
				//std::cout << "chunk created " << this->chunks.size() << std::endl;
				//std::cout << "chunk added " << key << std::endl;
				this->chunks.emplace(key, new ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, detail, this->textureAtlas, this->chunks, this->chunksMutex));

				std::string keyLeft = std::to_string((int)x - 1) + "|" + std::to_string((int)y);
				if (this->chunks.find(keyLeft) != this->chunks.end()) {
					this->chunks.at(keyLeft)->status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyRight = std::to_string((int)x + 1) + "|" + std::to_string((int)y);
				if (this->chunks.find(keyRight) != this->chunks.end()) {
					this->chunks.at(keyRight)->status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyForward = std::to_string((int)x) + "|" + std::to_string((int)y + 1);
				if (this->chunks.find(keyForward) != this->chunks.end()) {
					this->chunks.at(keyForward)->status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyBackward = std::to_string((int)x) + "|" + std::to_string((int)y - 1);
				if (this->chunks.find(keyBackward) != this->chunks.end()) {
					this->chunks.at(keyBackward)->status = ChunkStatus::TERAIN_GENERATED;
				}
				break;
			}
			else if (this->chunks.at(key)->detailMultiplier != detail) {
				//replace chunk with different detail level
				this->chunks.at(key)->status = ChunkStatus::NONE;
				this->chunks.at(key)->detailMultiplier = detail;
				this->chunks.at(key)->generateTerain();

				std::string keyLeft = std::to_string((int)x - 1) + "|" + std::to_string((int)y);
				if (this->chunks.find(keyLeft) != this->chunks.end() && this->chunks.at(keyLeft)->detailMultiplier == detail) {
					this->chunks.at(keyLeft)->status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyRight = std::to_string((int)x + 1) + "|" + std::to_string((int)y);
				if (this->chunks.find(keyRight) != this->chunks.end() && this->chunks.at(keyRight)->detailMultiplier == detail) {
					this->chunks.at(keyRight)->status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyForward = std::to_string((int)x) + "|" + std::to_string((int)y + 1);
				if (this->chunks.find(keyForward) != this->chunks.end() && this->chunks.at(keyForward)->detailMultiplier == detail) {
					this->chunks.at(keyForward)->status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyBackward = std::to_string((int)x) + "|" + std::to_string((int)y - 1);
				if (this->chunks.find(keyBackward) != this->chunks.end() && this->chunks.at(keyBackward)->detailMultiplier == detail) {
					this->chunks.at(keyBackward)->status = ChunkStatus::TERAIN_GENERATED;
				}

			}
			lock.unlock();
		}
	}


	std::cout << "chunks " << this->chunks.size() << std::endl;

	std::shared_lock<std::shared_mutex> lock(chunksMutex);
	for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
		for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);
			if (distance > chunkCount) continue;

			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			if (this->chunks.find(key) != this->chunks.end()) {
				auto currentChunk = this->chunks.at(key);
				std::unique_lock<std::mutex> chunkLock(currentChunk->chunkLock, std::defer_lock);
				if (chunkLock.try_lock()) {
					if (currentChunk->status == ChunkStatus::TERAIN_GENERATED) {
						currentChunk->generateDecorations();
					}
					chunkLock.unlock();
				}
			}

		}
	}
	lock.unlock();
}

using namespace std::chrono_literals;
void ChunkManager::CreateChunkMesh() {
	while (isRunning) {
		generateChunks();

		std::shared_lock<std::shared_mutex> lock(this->chunksMutex);
		for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
			for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
				float deltaX = (x + originX);
				float deltaZ = (y + originZ);
				float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);
				if (distance > chunkCount) continue;

				std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
				if (this->chunks.find(key) == this->chunks.end()) continue;

				auto currentChunk = this->chunks.at(key);
				std::unique_lock<std::mutex> lock2(currentChunk->chunkLock, std::defer_lock);
				if (lock2.try_lock()) {
					if (currentChunk->status == ChunkStatus::DECORATIONS_GENERATED) {
					currentChunk->generateMesh();
					}
					lock2.unlock();
				}
				
			}
		}
		lock.unlock();
		std::this_thread::sleep_for(10ms);
	}
}


void ChunkManager::CreateEntities() {
	std::shared_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
	if (lock.try_lock()) {
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			std::unique_lock<std::mutex> chunkLock(chunk->chunkLock, std::defer_lock);
			if (chunkLock.try_lock()) {
				if (chunk->status == ChunkStatus::MESH_GENERATED) {
					if (chunk->mesh == nullptr) continue;
					auto vao = new glp::Vao(*chunk->mesh, false);

					if (chunk->chunkEntity != NULL) {
						delete chunk->chunkEntity->model;

						chunk->chunkEntity->setModel(vao);
						chunk->status = ChunkStatus::RENDERED;
						delete chunk->mesh;
						chunk->mesh = nullptr;
						continue;
					}
					auto entity = new glp::Entity(vao, &this->shader, 1);
					entity->setX(chunk->chunkX * chunk->chunkWidth);
					entity->setZ(chunk->chunkZ * chunk->chunkWidth);
					entity->addTexture(&this->textureAtlas.texture);

					chunk->chunkEntity = entity;
					chunk->status = ChunkStatus::RENDERED;

					delete chunk->mesh;
					chunk->mesh = nullptr;
				}
				chunkLock.unlock();
			}
			
		}
		lock.unlock();
	}
}
