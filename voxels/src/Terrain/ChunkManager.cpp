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
		bool chunkExists = false;
		for (int i = 0; i < this->chunks.size(); i++) {
			if (this->chunks[i].chunkX == x && this->chunks[i].chunkZ == y) {
				chunkExists = true;
			}
		}
		if (!chunkExists) {
			this->chunks.push_back(ChunkGenerator(x, y, chunkWidth, chunkHeight, this->textureAtlas));
		}
	}
	
	/*for (int x = 0; x < chunks; x++) {
		for (int y = 0; y < chunks; y++) {
			this->chunks.push_back(ChunkGenerator(x, y, chunkWidth, chunkHeight, this->textureAtlas));
		}
	}*/
}

void ChunkManager::CreateEntities() {
	for (int i = 0; i < this->chunks.size(); i++) {
		if (this->chunks[i].status == ChunkStatus::TERAIN_GENERATED) {
			auto mesh = this->chunks[i].generateMesh();
			auto vao = new glp::Vao(mesh, false);
			//this->models.push_back(glp::Vao(mesh, false));

			auto entity = glp::Entity(*vao, &this->shader, 1);
			entity.setX(this->chunks[i].chunkX * this->chunks[i].chunkWidth);
			entity.setZ(this->chunks[i].chunkZ * this->chunks[i].chunkWidth);
			entity.addTexture(&this->textureAtlas.texture);
			this->entities.push_back(entity);
			return;
		}
	}
}
