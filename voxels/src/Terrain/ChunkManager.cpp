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
	//std::cout << std::to_string(x) + " " + std::to_string(y) << std::endl;
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
			this->chunks.emplace(key, ChunkGenerator(x, y, this->chunkWidth, this->chunkHeight, this->textureAtlas));
			//std::cout << "adding chunk" + std::to_string(x) + " " + std::to_string(y) << std::endl;
		}
	}
	this->CreateEntities();
}

void ChunkManager::CreateEntities() {
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = &i->second;
		if (chunk->status == ChunkStatus::TERAIN_GENERATED) {
			//std::cout << "adding entity" + std::to_string(chunk.chunkX) + " " + std::to_string(chunk.chunkZ) + "  " + std::to_string(chunk.status) << std::endl;
			auto mesh = chunk->generateMesh();
			auto vao = new glp::Vao(mesh, false);

			auto entity = glp::Entity(*vao, &this->shader, 1);
			entity.setX(chunk->chunkX * chunk->chunkWidth);
			entity.setZ(chunk->chunkZ * chunk->chunkWidth);
			entity.addTexture(&this->textureAtlas.texture);
			this->entities.push_back(entity);
		}
	}
}
