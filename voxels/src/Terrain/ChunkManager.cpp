#include "ChunkManager.h"

ChunkManager::ChunkManager(int chunks, int chunkWidth, int chunkHeight):
	shader("res/shaders/main.shader"),
	textureAtlas("res/textures/test.png", glp::Texture::FILTER::LINEAR, 16) {

	float currentChunkLayer = 0;
	float angle = 0;
	while (currentChunkLayer < chunks) {
		int x = 0;
		int y = 0;
		if (currentChunkLayer == 0) {
			currentChunkLayer++;
		}
		else {
			x = glm::cos(angle) * currentChunkLayer;
			y = glm::sin(angle) * currentChunkLayer;
			angle += (glm::pi<float>() * 2) / (14 * currentChunkLayer);
			if (angle > (glm::pi<float>() * 2) * currentChunkLayer) {
				currentChunkLayer++;
			}
		}
		std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
		if (this->chunks.find(key) == this->chunks.end()) {
			this->chunks.emplace(key, ChunkGenerator(x, y, chunkWidth, chunkHeight, this->textureAtlas));
		}
	}
	
	/*for (int x = 0; x < chunks; x++) {
		for (int y = 0; y < chunks; y++) {
			std::string key = std::to_string((int)x) + "|" + std::to_string((int)y);
			this->chunks.emplace(key, ChunkGenerator(x, y, chunkWidth, chunkHeight, this->textureAtlas));
		}
	}*/
}

void ChunkManager::CreateEntities() {
	for (auto i = this->chunks.begin(); i != this->chunks.end(); i++) {
		auto chunk = i->second;
		if (chunk.status == ChunkStatus::TERAIN_GENERATED) {
			auto mesh = chunk.generateMesh();
			auto vao = new glp::Vao(mesh, false);
			//this->models.push_back(glp::Vao(mesh, false));

			auto entity = glp::Entity(*vao, &this->shader, 1);
			entity.setX(chunk.chunkX * chunk.chunkWidth);
			entity.setZ(chunk.chunkZ * chunk.chunkWidth);
			entity.addTexture(&this->textureAtlas.texture);
			this->entities.push_back(entity);
		}
	}
	/*for (int i = 0; i < this->chunks.size(); i++) {
		if (this->chunks[i].status == ChunkStatus::TERAIN_GENERATED) {
			auto mesh = this->chunks[i].generateMesh();
			auto vao = new glp::Vao(mesh, false);
			//this->models.push_back(glp::Vao(mesh, false));

			auto entity = glp::Entity(*vao, &this->shader, 1);
			entity.setX(this->chunks[i].chunkX * this->chunks[i].chunkWidth);
			entity.setZ(this->chunks[i].chunkZ * this->chunks[i].chunkWidth);
			entity.addTexture(&this->textureAtlas.texture);
			this->entities.push_back(entity);
		}
	}*/
}
