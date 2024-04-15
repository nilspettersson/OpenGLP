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
	//this->chunkThread = std::thread(&ChunkManager::CreateChunkMesh, this);
	//this->meshThread.detach();
}

void ChunkManager::updateChunks(int originX, int originZ) {
	this->originX = originX;
	this->originZ = originZ;

	this->CreateEntities();

	std::unique_lock<std::mutex> lock(chunksMutex, std::defer_lock); // Initialize without locking
	if (lock.try_lock()) {
		auto chunksToBeRemoved = std::vector<ChunkGenerator*>();
		int removeCount = 6;
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			float deltaX = (chunk->chunkX + originX);
			float deltaZ = (chunk->chunkZ + originZ);

			float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);

			if (distance > chunkCount) {
				for (int j = 0; j < this->entities.size(); j++) {
					if (this->entities[j] == chunk->chunkEntity) {
						chunksToBeRemoved.push_back(this->chunks.at(i->first));
						delete this->entities[j]->model;
						this->entities[j]->model = nullptr;
						delete this->entities[j];
						chunk->chunkEntity = nullptr;
						this->entities.erase(this->entities.begin() + j);
						break;
					}
				}
				removeCount--;
				if (removeCount == 0) {
					break;
				}
			}
		}

		for (auto chunk : chunksToBeRemoved) {
			std::string key = std::to_string((int)chunk->chunkX) + "|" + std::to_string((int)chunk->chunkZ);
			delete chunk;
			this->chunks.erase(key);
			//std::cout << "chunk removed " << this->chunks.size() << std::endl;
		}
		lock.unlock();
	}
}

void ChunkManager::generateChunks() {
	std::unique_lock<std::mutex> lock(chunksMutex);
	bool chunkCreated = false;
	for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
		for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float detail = 1;
			float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);

			if (distance > chunkCount) continue;

			if (distance > 20) {
				detail /= 16;
			}
			else if (distance > 14) {
				detail /= 8;
			}
			else if (distance > 8) {
				detail /= 4;
			}
			else if (distance > 4) {
				detail /= 2;
			}
			//detail = 1;

			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			if (this->chunks.find(key) == this->chunks.end()) {
				chunkCreated = true;
				//std::cout << "chunk created " << this->chunks.size() << std::endl;
				this->chunks.emplace(key, new ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, detail, this->textureAtlas, this->chunks));

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

		}
	}
	//lock.unlock();
	if (chunkCreated) {
		std::cout << "chunk created " << this->chunks.size() << std::endl;
	}


	std::cout << "chunks " << this->chunks.size() << std::endl;
	std::cout << "entities " << this->entities.size() << std::endl;
	//std::unique_lock<std::mutex> lock2(chunksMutex);
	
	for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
		for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);
			if (distance > chunkCount) continue;

			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			if (this->chunks.find(key) != this->chunks.end() && this->chunks.at(key)->status == ChunkStatus::TERAIN_GENERATED) {
				this->chunks.at(key)->generateDecorations();
				//break;
			}

		}
	}



	lock.unlock();
}

using namespace std::chrono_literals;
void ChunkManager::CreateChunkMesh() {
	while (isRunning) {
		generateChunks();

		std::unique_lock<std::mutex> lock(chunksMutex);
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			if (chunk->status == ChunkStatus::DECORATIONS_GENERATED) {
				chunk->generateMesh();
			}
		}
		lock.unlock();
		std::this_thread::sleep_for(40ms);
	}
}


void ChunkManager::CreateEntities() {
	//std::unique_lock<std::mutex> lock(chunksMutex);
	//if (lock.try_lock()) {
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
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

				//std::cout << "create entity " << chunk->chunkX << " " << chunk->chunkZ << std::endl;
				auto entity = new glp::Entity(vao, &this->shader, 1);
				entity->setX(chunk->chunkX * chunk->chunkWidth);
				entity->setZ(chunk->chunkZ * chunk->chunkWidth);
				entity->addTexture(&this->textureAtlas.texture);
				this->entities.push_back(entity);

				chunk->chunkEntity = entity;
				chunk->status = ChunkStatus::RENDERED;

				delete chunk->mesh;
				chunk->mesh = nullptr;
				//break;
			}
		}
		//lock.unlock();
	//}
}
