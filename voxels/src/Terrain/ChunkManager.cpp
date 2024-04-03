#include "ChunkManager.h"

ChunkManager::ChunkManager(int chunks, int chunkWidth, int chunkHeight):
	shader("res/shaders/main.shader"),
	textureAtlas("res/textures/test.png", glp::Texture::FILTER::LINEAR, 16) {

	this->chunkWidth = chunkWidth;
	this->chunkHeight = chunkHeight;
	this->chunkCount = chunks;

	this->updateChunks(0, 0);
}

void ChunkManager::updateChunks(int originX, int originZ) {
	for (int x = -chunkCount / 2 - originX; x < chunkCount / 2 - originX; x++) {
		for (int y = -chunkCount / 2 - originZ; y < chunkCount / 2 - originZ; y++) {
			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			if (this->chunks.find(key) == this->chunks.end()) {
				this->chunks.emplace(key, ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, 1, this->textureAtlas, this->chunks));

				std::string keyLeft = std::to_string((int)x - 1) + "|" + std::to_string((int)y);
				if (this->chunks.find(keyLeft) != this->chunks.end()) {
					this->chunks.at(keyLeft).status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyRight = std::to_string((int)x + 1) + "|" + std::to_string((int)y);
				if (this->chunks.find(keyRight) != this->chunks.end()) {
					this->chunks.at(keyRight).status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyForward = std::to_string((int)x) + "|" + std::to_string((int)y + 1);
				if (this->chunks.find(keyForward) != this->chunks.end()) {
					this->chunks.at(keyForward).status = ChunkStatus::TERAIN_GENERATED;
				}

				std::string keyBackward = std::to_string((int)x) + "|" + std::to_string((int)y - 1);
				if (this->chunks.find(keyBackward) != this->chunks.end()) {
					this->chunks.at(keyBackward).status = ChunkStatus::TERAIN_GENERATED;
				}
			}
			
		}
	}

	/*float currentChunkLayer = 0;
	float angle = 0;
	while (currentChunkLayer < chunkCount) {
		int x = 0;
		int y = 0;
		if (currentChunkLayer == 0) {
			currentChunkLayer++;
			continue;
		}
		else {
			x = glm::cos(angle) * currentChunkLayer - originX;
			y = glm::sin(angle) * currentChunkLayer - originZ;
			angle += (glm::pi<float>() * 2) / (14 * currentChunkLayer);
			if (angle > (glm::pi<float>() * 2) * currentChunkLayer) {
				currentChunkLayer++;
			}
		}
		std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
		if (this->chunks.find(key) == this->chunks.end()) {
			float detail = 1;
			if (currentChunkLayer < 12) {
				detail /= 1;
			}
			else if (currentChunkLayer < 24) {
				detail /= 2;
			}
			else if (currentChunkLayer < 45) {
				detail /= 4;
			}
			else if (currentChunkLayer < 60) {
				detail /= 8;
			}
			else {
				detail /= 16;
			}
			this->chunks.emplace(key, ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, 1, this->textureAtlas, this->chunks));


			std::string keyLeft = std::to_string((int)x - 1) + "|" + std::to_string((int)y);
			if (this->chunks.find(keyLeft) != this->chunks.end()) {
				this->chunks.at(keyLeft).status = ChunkStatus::TERAIN_GENERATED;
			}

			std::string keyRight = std::to_string((int)x + 1) + "|" + std::to_string((int)y);
			if (this->chunks.find(keyRight) != this->chunks.end()) {
				this->chunks.at(keyRight).status = ChunkStatus::TERAIN_GENERATED;
			}

			std::string keyForward = std::to_string((int)x) + "|" + std::to_string((int)y + 1);
			if (this->chunks.find(keyForward) != this->chunks.end()) {
				this->chunks.at(keyForward).status = ChunkStatus::TERAIN_GENERATED;
			}

			std::string keyBackward = std::to_string((int)x) + "|" + std::to_string((int)y - 1);
			if (this->chunks.find(keyBackward) != this->chunks.end()) {
				this->chunks.at(keyBackward).status = ChunkStatus::TERAIN_GENERATED;
			}
		}
	}*/
	this->CreateEntities();

	//std::cout << "entity count " << this->entities.size() << std::endl;
	//std::cout << "chunk count " << this->chunks.size() << std::endl;

	auto chunksToBeRemoved = std::vector<ChunkGenerator>();
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = &i->second;
		float deltaX = (chunk->chunkX + originX);
		float deltaZ = (chunk->chunkZ + originZ);
		if (glm::sqrt(deltaX * deltaX + deltaZ * deltaZ) > chunkCount) {
			for (int j = 0; j < this->entities.size(); j++) {
				if (this->entities[j] == chunk->chunkEntity) {
					//TODO: remove chunk?
					chunksToBeRemoved.push_back(this->chunks.at(i->first));
					std::cout << "remove entity " << i->first << " x:" << deltaX << " z:" << deltaZ << std::endl;
					//delete chunk;
					this->entities.erase(this->entities.begin() + j);
					break;
				}
			}
			
		}
	}
	for (auto chunk : chunksToBeRemoved) {
		for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
			if (i->second.chunkEntity == chunk.chunkEntity) {
				this->chunks.erase(i->first);
				//delete& chunk;
				std::cout << "remove chunk done" << std::endl;
				break;
			}
		}
		
	}
}

void ChunkManager::CreateEntities() {
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = &i->second;
		if (chunk->status == ChunkStatus::TERAIN_GENERATED) {
			auto mesh = chunk->generateMesh();
			auto vao = new glp::Vao(mesh, false);

			/*if (chunk->chunkEntityIndex != -1) {
				this->entities[chunk->chunkEntityIndex].setModel(*vao);
				return;
			}*/

			if (chunk->chunkEntity != NULL) {
				chunk->chunkEntity->setModel(*vao);
				return;
			}

			/*for (int i = 0; i < this->entities.size(); i++) {
				if (&this->entities[i] == chunk->chunkEntity) {
					this->entities[i].setModel(*vao);
					return;
				}
			}*/
			std::cout << "create entity " << chunk->chunkX << " " << chunk->chunkZ << std::endl;
			auto entity = new glp::Entity(*vao, &this->shader, 1);
			entity->setX(chunk->chunkX * chunk->chunkWidth);
			entity->setZ(chunk->chunkZ * chunk->chunkWidth);
			entity->addTexture(&this->textureAtlas.texture);
			this->entities.push_back(entity);

			chunk->chunkEntity = entity;
		}
	}
}
