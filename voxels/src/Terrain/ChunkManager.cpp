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
	float currentChunkLayer = 0;
	float angle = 0;
	while (currentChunkLayer < chunkCount) {
		int x = 0;
		int y = 0;
		if (currentChunkLayer == 0) {
			currentChunkLayer++;
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
	}
	this->CreateEntities();
}

void ChunkManager::CreateEntities() {
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = &i->second;
		if (chunk->status == ChunkStatus::TERAIN_GENERATED) {
			auto mesh = chunk->generateMesh();
			auto vao = new glp::Vao(mesh, false);

			//TODO: improve performance
			for (int i = 0; i < this->entities.size(); i++) {
				if (this->entities[i].getX() == chunk->chunkX * chunk->chunkWidth && this->entities[i].getZ() == chunk->chunkZ * chunk->chunkWidth) {
					this->entities[i].setModel(*vao);
					return;
				}
			}

			auto entity = glp::Entity(*vao, &this->shader, 1);
			entity.setX(chunk->chunkX * chunk->chunkWidth);
			entity.setZ(chunk->chunkZ * chunk->chunkWidth);
			entity.addTexture(&this->textureAtlas.texture);
			this->entities.push_back(entity);
		}
	}
}
