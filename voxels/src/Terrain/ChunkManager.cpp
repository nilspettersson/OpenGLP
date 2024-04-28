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
void ChunkManager::spiral(std::function<void(int x, int y)> processChunk)
{
	//std::cout << this->originX << this->originZ << std::endl;
	int centerX = -this->originX; // Assuming originX is defined relative to the player
	int centerZ = -this->originZ; // Assuming originZ is defined relative to the player
	int layer = 1;
	int maxLayer = chunkCount; // Define how far out you want to generate

	// Generate the center chunk first
	processChunk(centerX, centerZ);

	// Spiral outward
	while (layer <= maxLayer) {
		int x = centerX - layer;
		int z = centerZ - layer;
		int sideLength = 2 * layer;

		// Top side (moving right)
		for (int i = 0; i < sideLength; ++i, ++x) {
			processChunk(x, z);
		}

		// Right side (moving down)
		for (int i = 0; i < sideLength; ++i, ++z) {
			processChunk(x, z);
		}

		// Bottom side (moving left)
		for (int i = 0; i < sideLength; ++i, --x) {
			processChunk(x, z);
		}

		// Left side (moving up)
		for (int i = 0; i < sideLength; ++i, --z) {
			processChunk(x, z);
		}

		// Move to the next layer
		layer++;
	}
}

void ChunkManager::addChunk(int x, int y) {
	float deltaX = (x + originX);
	float deltaZ = (y + originZ);
	float detail = 1;
	float distance = deltaX * deltaX + deltaZ * deltaZ;
	if (distance > chunkCount * chunkCount) return;

	int64_t key = getKey(x, y);
	if (this->chunks.find(key) == this->chunks.end()) {
		std::unique_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
		if (lock.try_lock()) {
			this->chunks.emplace(key, new ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, detail, this->textureAtlas, this->chunks, this->chunksMutex));
			lock.unlock();
			//limit--;
			//if (limit <= 0) break;
		}
	}
}

void ChunkManager::updateChunks(int originX, int originZ) {
	//std::cout << "updateChunks " << this->chunks.size() << std::endl;
	this->originX = originX;
	this->originZ = originZ;

	spiral(std::bind(&ChunkManager::addChunk, this, std::placeholders::_1, std::placeholders::_2));

	/*int limit = 120;
	for (int x = -chunkCount - originX; x < chunkCount - originX; x++) {
		if (limit <= 0) break;
		for (int y = -chunkCount - originZ; y < chunkCount - originZ; y++) {
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			float detail = 1;
			float distance = deltaX * deltaX + deltaZ * deltaZ;
			if (distance > chunkCount * chunkCount) continue;

			int64_t key = getKey(x, y);
			if (this->chunks.find(key) == this->chunks.end()) {
				std::unique_lock<std::shared_mutex> lock(chunksMutex, std::defer_lock);
				if (lock.try_lock()) {
					this->chunks.emplace(key, new ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, detail, this->textureAtlas, this->chunks, this->chunksMutex));
					lock.unlock();
					limit--;
					if (limit <= 0) break;
				}
			}
		}
	}*/



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
				auto key = getKey(chunk->chunkX, chunk->chunkZ);
				delete chunk;
				this->chunks.at(key) = nullptr;
				this->chunks.erase(key);
			}
			lock2.unlock();
		}
	}
}

void ChunkManager::generateChunk(int x, int y) {
	float deltaX = (x + originX);
	float deltaZ = (y + originZ);
	float detail = 1;

	float distance = deltaX * deltaX + deltaZ * deltaZ;
	if (distance > chunkCount * chunkCount) return;

	if (distance > 20 * 20) {
		detail /= 16;
	}
	else if (distance > 14 * 14) {
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
					this->chunks.at(keyLeft)->status = ChunkStatus::TERAIN_GENERATED;
				}
				auto keyRight = getKey(x + 1, y);
				if (this->chunks.find(keyRight) != this->chunks.end() && this->chunks.at(keyRight)->status != ChunkStatus::NONE) {
					this->chunks.at(keyRight)->status = ChunkStatus::TERAIN_GENERATED;
				}
				auto keyForward = getKey(x, y + 1);
				if (this->chunks.find(keyForward) != this->chunks.end() && this->chunks.at(keyForward)->status != ChunkStatus::NONE) {
					this->chunks.at(keyForward)->status = ChunkStatus::TERAIN_GENERATED;
				}
				auto keyBackward = getKey(x, y - 1);
				if (this->chunks.find(keyBackward) != this->chunks.end() && this->chunks.at(keyBackward)->status != ChunkStatus::NONE) {
					this->chunks.at(keyBackward)->status = ChunkStatus::TERAIN_GENERATED;
				}
				//break;
			}
		}
		else {
			if (this->chunks.at(key)->detailMultiplier != detail) {
				//replace chunk with different detail level
				std::unique_lock<std::shared_mutex> chunkLock(this->chunks.at(key)->chunkLock);
				this->chunks.at(key)->status = ChunkStatus::NONE;
				this->chunks.at(key)->detailMultiplier = detail;
				this->chunks.at(key)->generateTerain();
				chunkLock.unlock();

				auto keyLeft = getKey(x - 1, y);
				if (this->chunks.find(keyLeft) != this->chunks.end() && this->chunks.at(keyLeft)->detailMultiplier == detail) {
					this->chunks.at(keyLeft)->status = ChunkStatus::TERAIN_GENERATED;
				}
				auto keyRight = getKey(x + 1, y);
				if (this->chunks.find(keyRight) != this->chunks.end() && this->chunks.at(keyRight)->detailMultiplier == detail) {
					this->chunks.at(keyRight)->status = ChunkStatus::TERAIN_GENERATED;
				}
				auto keyForward = getKey(x, y + 1);
				if (this->chunks.find(keyForward) != this->chunks.end() && this->chunks.at(keyForward)->detailMultiplier == detail) {
					this->chunks.at(keyForward)->status = ChunkStatus::TERAIN_GENERATED;
				}
				auto keyBackward = getKey(x, y - 1);
				if (this->chunks.find(keyBackward) != this->chunks.end() && this->chunks.at(keyBackward)->detailMultiplier == detail) {
					this->chunks.at(keyBackward)->status = ChunkStatus::TERAIN_GENERATED;
				}
			}
		}
	}
	shared_lock.unlock();
}

void ChunkManager::generateDecorations(int x, int y) {
	float deltaX = (x + originX);
	float deltaZ = (y + originZ);
	float distance = deltaX * deltaX + deltaZ * deltaZ;
	if (distance > chunkCount * chunkCount) return;

	auto key = getKey(x, y);
	std::shared_lock<std::shared_mutex> lock2(chunksMutex);
	if (this->chunks.find(key) != this->chunks.end()) {
		auto currentChunk = this->chunks.at(key);

		if (currentChunk->status == ChunkStatus::TERAIN_GENERATED) {
			std::unique_lock<std::shared_mutex> chunkLock(currentChunk->chunkLock, std::defer_lock);
			if (chunkLock.try_lock()) {
				currentChunk->generateDecorations();
				chunkLock.unlock();
			}
		}
	}
}

void ChunkManager::generateChunks() {
	spiral(std::bind(&ChunkManager::generateChunk, this, std::placeholders::_1, std::placeholders::_2));
	spiral(std::bind(&ChunkManager::generateDecorations, this, std::placeholders::_1, std::placeholders::_2));
	
}

void ChunkManager::CreateMesh(int x, int y) {
	float deltaX = (x + originX);
	float deltaZ = (y + originZ);
	float distance = deltaX * deltaX + deltaZ * deltaZ;
	if (distance > chunkCount * chunkCount) return;

	auto key = getKey(x, y);
	std::shared_lock<std::shared_mutex> lock(this->chunksMutex);
	if (this->chunks.find(key) == this->chunks.end() || this->chunks.at(key)->status != ChunkStatus::DECORATIONS_GENERATED) return;
	std::unique_lock<std::shared_mutex> mainChunkLock(this->chunks.at(key)->chunkLock, std::defer_lock);
	if (!mainChunkLock.try_lock()) return;

	std::vector<ChunkGenerator*> relevantChunks = {};
	bool allChunksFound = true;
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (dx == 0 && dy == 0) continue; //skip the current chunk
			int nx = x + dx;
			int ny = y + dy;
			auto nearKey = getKey(nx, ny);
			if (this->chunks.find(nearKey) != this->chunks.end()) {
				relevantChunks.push_back(this->chunks.at(nearKey));
			}
			else {
				allChunksFound = false;
				break;
			}
		}
	}
	if (!allChunksFound) {
		return;
	}

	std::vector<std::shared_lock<std::shared_mutex>> locks;

	bool allLocked = true;
	for (auto chunk : relevantChunks) {
		if (chunk->status == ChunkStatus::NONE || chunk->status == ChunkStatus::TERAIN_GENERATED) {
			allLocked = false;
			break;
		}
		std::shared_lock<std::shared_mutex> chunkLock(chunk->chunkLock, std::defer_lock);
		if (!chunkLock.try_lock()) {
			allLocked = false;
			break;
		}
		locks.push_back(std::move(chunkLock));
	}

	if (allLocked) {
		this->chunks.at(key)->generateMesh();
		//break;
		locks.clear();
	}
	else {
		locks.clear();
		//break;
	}
}

void ChunkManager::CreateChunkMesh() {
	while (isRunning) {
		generateChunks();
		spiral(std::bind(&ChunkManager::CreateMesh, this, std::placeholders::_1, std::placeholders::_2));
		std::this_thread::sleep_for(12ms);
	}
}


//hapens on main thread, no need to lock
void ChunkManager::CreateEntities() {
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = i->second;
		if (chunk->status == ChunkStatus::MESH_GENERATED) {
			if (chunk->mesh == nullptr) continue;

			auto vao = new glp::Vao(*chunk->mesh, false);
			if (chunk->chunkEntity != NULL) {
				delete chunk->chunkEntity->model;
				chunk->chunkEntity->setModel(vao);
			}
			else {
				auto entity = new glp::Entity(vao, &this->shader, 1);
				entity->setX(chunk->chunkX * chunk->chunkWidth);
				entity->setZ(chunk->chunkZ * chunk->chunkWidth);
				entity->addTexture(&this->textureAtlas.texture);

				chunk->chunkEntity = entity;
			}
			chunk->status = ChunkStatus::RENDERED;
			delete chunk->mesh;
			chunk->mesh = nullptr;
		}
	}
}
