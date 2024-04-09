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
			float deltaX = (x + originX);
			float deltaZ = (y + originZ);
			/*deltaX = x;
			deltaZ = y;*/
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
			else if (this->chunks.at(key).detailMultiplier != detail) {
				//replace chunk with different detail level
				/*this->chunks.at(key).status = ChunkStatus::NONE;
				this->chunks.at(key).detailMultiplier = detail;
				this->chunks.at(key).generateTerain();

				for (int j = 0; j < this->entities.size(); j++) {
					if (this->entities[j] == this->chunks.at(key).chunkEntity) {
						delete& this->chunks.at(key).chunkEntity->getModel();
						delete this->chunks.at(key).chunkEntity;
						this->chunks.erase(key);
						this->entities.erase(this->entities.begin() + j);
						break;
					}
				}*/
			}
			
		}
	}
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


					chunksToBeRemoved.push_back(this->chunks.at(i->first));
					//std::cout << "remove entity " << i->first << " x:" << deltaX << " z:" << deltaZ << std::endl;
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
				delete &chunk.chunkEntity->getModel();
				delete chunk.chunkEntity;
				std::cout << "chunk removed" << std::endl;
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
			auto vao = new glp::Vao(*mesh, false);

			/*if (chunk->chunkEntityIndex != -1) {
				this->entities[chunk->chunkEntityIndex].setModel(*vao);
				return;
			}*/

			if (chunk->chunkEntity != NULL) {
				chunk->chunkEntity->setModel(*vao);
				delete mesh;
				continue;
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

			delete mesh;
			break;
		}
	}
}
