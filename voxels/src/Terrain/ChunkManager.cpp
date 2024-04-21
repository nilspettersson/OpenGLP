#include "ChunkManager.h"
#include <thread>

using namespace std::chrono_literals;
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

	const auto processorCount = glm::max(std::thread::hardware_concurrency(), 4u) - 1;

	for (int i = 0; i < processorCount; i++) {
		this->meshThreads.push_back(std::thread(&ChunkManager::CreateChunkMesh, this));
	}
}

ChunkManager::~ChunkManager() {
	isRunning = false;
	for (auto &thread: meshThreads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

}
void ChunkManager::updateChunks(int originX, int originZ) {
	//std::cout << "updateChunks " << this->chunks.size() << std::endl;
	this->originX = originX;
	this->originZ = originZ;

	//std::cout << "key " << getKey(2, 1) << std::endl;
	for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
		for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float detail = 1;
			float distance = deltaX * deltaX + deltaZ * deltaZ;
			if (distance > chunkCount * chunkCount) continue;

			int64_t key = getKey(x, y);
			std::unique_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
			if (this->chunks.find(key) == this->chunks.end()) {
			if (lock.try_lock()) {
					this->chunks.emplace(key, new ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, detail, this->textureAtlas, this->chunks, this->chunksMutex));
					lock.unlock();
					break;
				}
			}
		}
	}



	this->CreateEntities();

	std::shared_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
	auto chunksToBeRemoved = std::vector<ChunkGenerator*>();
	if (lock.try_lock()) {
		int removeCount = 20;
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			float deltaX = (chunk->chunkX + originX);
			float deltaZ = (chunk->chunkZ + originZ);

			float distance = deltaX * deltaX + deltaZ * deltaZ;

			if (distance > chunkCount * chunkCount) {
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
				auto key = getKey(chunk->chunkX, chunk->chunkZ);
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
	//std::unique_lock<std::shared_mutex> lock(chunksMutex);
	//if (lock.try_lock()) {
		for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
			for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
				float deltaX = (x + originX);
				float deltaZ = (y + originZ);
				float detail = 1;

				float distance = deltaX * deltaX + deltaZ * deltaZ;
				if (distance > chunkCount * chunkCount) continue;

				if (distance > 20 * 20) {
					detail /= 16;
				}
				else if (distance > 12 * 12) {
					detail /= 8;
				}
				else if (distance > 8 * 8) {
					detail /= 4;
				}
				else if (distance > 4 * 4) {
					detail /= 2;
				}
				detail = 1;

				auto key = getKey(x, y);
				bool chunkEmpty = false;
				std::shared_lock<std::shared_mutex> shared_lock(chunksMutex);
				if (this->chunks.find(key) != this->chunks.end()) {
					auto chunk = this->chunks.at(key);
					if (chunk->status == ChunkStatus::NONE) {
						std::unique_lock<std::shared_mutex> chunk_lock(chunk->chunkLock, std::defer_lock);
						if (chunk_lock.try_lock()) {
							chunk->detailMultiplier = detail;
							chunk->generateTerain();
							chunk_lock.unlock();


							auto keyLeft = getKey(x - 1, y);
							if (this->chunks.find(keyLeft) != this->chunks.end() && this->chunks.at(keyLeft)->status != ChunkStatus::NONE) {
								std::unique_lock<std::shared_mutex> chunk_lock2(this->chunks.at(keyLeft)->chunkLock, std::defer_lock);
								chunk_lock2.lock();
								this->chunks.at(keyLeft)->status = ChunkStatus::TERAIN_GENERATED;
								chunk_lock2.unlock();
							}

							auto keyRight = getKey(x + 1, y);
							if (this->chunks.find(keyRight) != this->chunks.end() && this->chunks.at(keyRight)->status != ChunkStatus::NONE) {
								std::unique_lock<std::shared_mutex> chunk_lock2(this->chunks.at(keyRight)->chunkLock, std::defer_lock);
								chunk_lock2.lock();
								this->chunks.at(keyRight)->status = ChunkStatus::TERAIN_GENERATED;
								chunk_lock2.unlock();
							}

							auto keyForward = getKey(x, y + 1);
							if (this->chunks.find(keyForward) != this->chunks.end() && this->chunks.at(keyForward)->status != ChunkStatus::NONE) {
								std::unique_lock<std::shared_mutex> chunk_lock2(this->chunks.at(keyForward)->chunkLock, std::defer_lock);
								chunk_lock2.lock();
								this->chunks.at(keyForward)->status = ChunkStatus::TERAIN_GENERATED;
								chunk_lock2.unlock();
							}

							auto keyBackward = getKey(x, y - 1);
							if (this->chunks.find(keyBackward) != this->chunks.end() && this->chunks.at(keyBackward)->status != ChunkStatus::NONE) {
								std::unique_lock<std::shared_mutex> chunk_lock2(this->chunks.at(keyBackward)->chunkLock, std::defer_lock);
								chunk_lock2.lock();
								this->chunks.at(keyBackward)->status = ChunkStatus::TERAIN_GENERATED;
								chunk_lock2.unlock();
							}




							break;
						}
					}
					else if (chunk->status != ChunkStatus::NONE) {
						std::lock_guard<std::shared_mutex> chunkLock(this->chunks.at(key)->chunkLock);
						if (this->chunks.at(key)->detailMultiplier != detail) {
							//replace chunk with different detail level
							this->chunks.at(key)->status = ChunkStatus::NONE;
							this->chunks.at(key)->detailMultiplier = detail;
							this->chunks.at(key)->generateTerain();

							/*std::string keyLeft = std::to_string((int)x - 1) + "|" + std::to_string((int)y);
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
							}*/
						}
					}
				}
				shared_lock.unlock();
			}
		}

	//std::cout << "chunks " << this->chunks.size() << std::endl;

	std::shared_lock<std::shared_mutex> lock2(chunksMutex, std::defer_lock);
	if (lock2.try_lock()) {
		for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
			for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
				float deltaX = (x + originX);
				float deltaZ = (y + originZ);
				float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);
				if (distance > chunkCount) continue;

				auto key = getKey(x, y);
				if (this->chunks.find(key) != this->chunks.end()) {
					auto currentChunk = this->chunks.at(key);
					std::unique_lock<std::shared_mutex> chunkLock(currentChunk->chunkLock, std::defer_lock);
					if (chunkLock.try_lock()) {
						if (currentChunk->status == ChunkStatus::TERAIN_GENERATED) {
							currentChunk->generateDecorations();
						}
						chunkLock.unlock();
					}
				}

			}
		}
		lock2.unlock();
	}
	
}

void ChunkManager::CreateChunkMesh() {
	while (isRunning) {
		generateChunks();

		std::shared_lock<std::shared_mutex> lock(this->chunksMutex);
		for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
			for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
				bool done = false;
				while (!done) {
					done = true;
					float deltaX = (x + originX);
					float deltaZ = (y + originZ);
					float distance = glm::sqrt(deltaX * deltaX + deltaZ * deltaZ);
					if (distance > chunkCount) continue;

					auto key = getKey(x, y);
					if (this->chunks.find(key) == this->chunks.end()) continue;

					std::vector<ChunkGenerator*> relevantChunks = {};
					for (int dx = -1; dx <= 1; dx++) {
						for (int dy = -1; dy <= 1; dy++) {
							int nx = x + dx;
							int ny = y + dy;
							auto key = getKey(nx, ny);
							if (this->chunks.find(key) != this->chunks.end()) {
								relevantChunks.push_back(this->chunks.at(key));
							}
						}
					}

					std::vector<std::unique_lock<std::shared_mutex>> locks;

					bool allLocked = true;
					for (auto chunk : relevantChunks) {
						if (this->chunks.at(key)->status != ChunkStatus::DECORATIONS_GENERATED) {
							allLocked = false;
							break;
						}
						std::unique_lock<std::shared_mutex> chunkLock(chunk->chunkLock, std::defer_lock);
						if (!chunkLock.try_lock()) {
							//done = false;
							allLocked = false;
							break;
						}
						locks.push_back(std::move(chunkLock));
					}

					if (allLocked) {
						if (this->chunks.at(key)->status == ChunkStatus::DECORATIONS_GENERATED) {
							this->chunks.at(key)->generateMesh();
						}
						locks.clear();
					}
					else {
						locks.clear();
						//std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
				}
				
			}
		}
		lock.unlock();
		std::this_thread::sleep_for(60ms);
	}
}


void ChunkManager::CreateEntities() {
	std::shared_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
	if (lock.try_lock()) {
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			auto chunk = i->second;
			std::unique_lock<std::shared_mutex> chunkLock(chunk->chunkLock, std::defer_lock);
			if (chunk->status == ChunkStatus::MESH_GENERATED) {
				if (chunkLock.try_lock()) {
				
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

					chunkLock.unlock();
				}
			}
			
		}
		lock.unlock();
	}
}
