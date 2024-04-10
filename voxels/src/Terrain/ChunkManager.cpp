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


	std::unique_lock<std::mutex> lock(chunksMutex);
	auto chunksToBeRemoved = std::vector<ChunkGenerator*>();
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = i->second;
		float deltaX = (chunk->chunkX + originX);
		float deltaZ = (chunk->chunkZ + originZ);
		if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > chunkCount) {
			for (int j = 0; j < this->entities.size(); j++) {
				if (this->entities[j] == chunk->chunkEntity) {
					chunksToBeRemoved.push_back(this->chunks.at(i->first));
					delete chunk->chunkEntity;
					this->entities.erase(this->entities.begin() + j);
					break;
				}
			}

		}
	}

	for (auto chunk : chunksToBeRemoved) {
		std::string key = std::to_string((int)chunk->chunkX) + "|" + std::to_string((int)chunk->chunkZ);
		delete chunk;
		this->chunks.erase(key);
		std::cout << "chunk removed " << this->chunks.size() << std::endl;
	}

	lock.unlock();
}

void ChunkManager::generateChunks() {
	std::unique_lock<std::mutex> lock(chunksMutex);
	for (int x = -chunkCount / 2 - originX; x < chunkCount / 2 - originX; x++) {
		for (int y = -chunkCount / 2 - originZ; y < chunkCount / 2 - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float detail = 1;
			if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > 96) {
				detail /= 16;
			}
			else if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > 48) {
				detail /= 8;
			}
			else if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > 24) {
				detail /= 4;
			}
			else if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > 12) {
				detail /= 2;
			}

			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			if (this->chunks.find(key) == this->chunks.end()) {

				this->chunks.emplace(key, new ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, 1, this->textureAtlas, this->chunks));

				/*std::string keyLeft = std::to_string((int)x - 1) + "|" + std::to_string((int)y);
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
				}*/
			}
			else if (this->chunks.at(key)->detailMultiplier != detail) {
				//replace chunk with different detail level
				/*this->chunks.at(key)->status = ChunkStatus::NONE;
				this->chunks.at(key)->detailMultiplier = detail;
				this->chunks.at(key)->generateTerain();*/

				/*for (int j = 0; j < this->entities.size(); j++) {
					if (this->entities[j] == this->chunks.at(key)->chunkEntity) {
						delete& this->chunks.at(key)->chunkEntity->getModel();
						delete this->chunks.at(key)->chunkEntity;
						this->chunks.erase(key);
						this->entities.erase(this->entities.begin() + j);
						break;
					}
				}*/
			}

		}
	}
	//std::cout << "entity count " << this->entities.size() << std::endl;
	//std::cout << "chunk count " << this->chunks.size() << std::endl;

	/*auto chunksToBeRemoved = std::vector<ChunkGenerator*>();
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = i->second;
		float deltaX = (chunk->chunkX + originX);
		float deltaZ = (chunk->chunkZ + originZ);
		if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > chunkCount) {
			for (int j = 0; j < this->entities.size(); j++) {
				if (this->entities[j] == chunk->chunkEntity) {
					chunksToBeRemoved.push_back(this->chunks.at(i->first));
					delete chunk->chunkEntity;
					this->entities.erase(this->entities.begin() + j);
					break;
				}
			}

		}
	}

	for (auto chunk : chunksToBeRemoved) {
		//delete chunk->chunkEntity;

		std::string key = std::to_string((int)chunk->chunkX) + "|" + std::to_string((int)chunk->chunkZ);
		//delete this->chunks.at(key);
		delete chunk;
		this->chunks.erase(key);
		std::cout << "chunk removed " << this->chunks.size() << std::endl;
	}*/

	lock.unlock();
}

using namespace std::chrono_literals;
void ChunkManager::CreateChunkMesh() {
	while (isRunning) {
		generateChunks();

		//std::cout << "CreateChunkMesh " << std::endl;
		std::unique_lock<std::mutex> lock(chunksMutex);
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			if (chunk->status == ChunkStatus::TERAIN_GENERATED) {
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
				//if (chunk->chunkEntity != NULL) continue;
				//std::cout << "create vao " << std::endl;
				if (chunk->mesh == nullptr) continue;
				auto vao = new glp::Vao(*chunk->mesh, false);

				if (chunk->chunkEntity != NULL) {
					chunk->chunkEntity->setModel(*vao);
					chunk->status = ChunkStatus::RENDERED;
					/*delete chunk->mesh;
					chunk->mesh = nullptr;*/
					continue;
				}

				//std::cout << "create entity " << chunk->chunkX << " " << chunk->chunkZ << std::endl;
				auto entity = new glp::Entity(*vao, &this->shader, 1);
				entity->setX(chunk->chunkX * chunk->chunkWidth);
				entity->setZ(chunk->chunkZ * chunk->chunkWidth);
				entity->addTexture(&this->textureAtlas.texture);
				this->entities.push_back(entity);

				chunk->chunkEntity = entity;
				chunk->status = ChunkStatus::RENDERED;

				/*delete chunk->mesh;
				chunk->mesh = nullptr;*/
				break;
			}
		}
		//lock.unlock();
	//}
}
