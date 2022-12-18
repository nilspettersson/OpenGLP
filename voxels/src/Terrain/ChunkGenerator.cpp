#include "ChunkGenerator.h"

ChunkGenerator::ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, TextureAtlas& textureAtlas): textureAtlas(textureAtlas) {
	this->chunkWidth = chunkWidth;
	this->maxHeight = maxHeight;
	this->chunkX = chunkX;
	this->chunkZ = chunkZ;
	this->cells = {};

	this->status = ChunkStatus::NONE;
	this->generateTerain();
}

int ChunkGenerator::GetTerainHeight(float x, float z, FastNoiseLite noise) {
	noise.SetFractalOctaves(4);
	noise.SetFractalGain(0.6);
	noise.SetFractalLacunarity(2);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	

	float xValue = (x + this->chunkX * this->chunkWidth);
	float zValue = (z + this->chunkZ * this->chunkWidth);
	float value = noise.GetNoise(xValue * 0.4, zValue * 0.4);
	value = (value + 1) / 2;
	value *= 0.2;

	value *= this->maxHeight;
	value += 1;

	return value;
}

void ChunkGenerator::generateTerain() {
	// Create and configure FastNoise object
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

	

	for (int x = 0; x < this->chunkWidth; x++) {
		this->cells.push_back({});
		for (int z = 0; z < this->chunkWidth; z++) {
			int terainHeight = this->GetTerainHeight((float)x, (float)z, noise);
			this->cells[x].push_back({});
			for (int y = 0; y < this->maxHeight; y++) {
				int block = 0;
				if (y <= terainHeight) {
					block = 1;
				}
				this->cells[x][z].push_back(block);
			}
		}
	}
	this->status = ChunkStatus::TERAIN_GENERATED;
}

float T(float VertexPosition, float position, float size) {
	return VertexPosition * size + position;
}


enum PlaneType {
	VERTICALX,
	VERTICALZ,
	HORIZONTAL
};
void createPlane(PlaneType planeType, std::vector<float>& vertices, std::vector<unsigned int>& indices, float x, float y, float z, float size, TextureCoordinates textureCoordinates, float light) {
	std::vector<float> planeVertices;
	if (planeType == PlaneType::VERTICALX) {
		planeVertices = {
			T(-0.5f, x, size), T(-0.5f, y, size), T(0.0f, z, size), textureCoordinates.x1, textureCoordinates.y1, light, 1.0f, 1.0f,
			T(0.5f, x, size), T(-0.5f, y, size), T(0.0f, z, size), textureCoordinates.x2, textureCoordinates.y1, light, 1.0f, 1.0f,
			T(0.5f, x, size), T(0.5f, y, size), T(0.0f, z, size), textureCoordinates.x2, textureCoordinates.y2, light, 1.0f, 1.0f,
			T(-0.5f, x, size), T(0.5f, y, size), T(0.0f, z, size), textureCoordinates.x1, textureCoordinates.y2, light, 1.0f, 1.0f,
		};
	}
	else if (planeType == PlaneType::VERTICALZ) {
		planeVertices = {
			T(0, x, size), T(-0.5f, y, size), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y1, light, 1.0f, 1.0f,
			T(0, x, size), T(-0.5f, y, size), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y1, light, 1.0f, 1.0f,
			T(0, x, size), T(0.5f, y, size), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y2, light, 1.0f, 1.0f,
			T(0, x, size), T(0.5f, y, size), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y2, light, 1.0f, 1.0f,
		};
	}
	else if (planeType == PlaneType::HORIZONTAL) {
		planeVertices = {
			T(-0.5f, x, size), T(0, y, size), T(-0.5f, z, size), textureCoordinates.x1, textureCoordinates.y1, light, 1.0f, 1.0f,
			T(0.5f, x, size), T(0, y, size), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y1, light, 1.0f, 1.0f,
			T(0.5f, x, size), T(0, y, size), T(0.5f, z, size), textureCoordinates.x2, textureCoordinates.y2, light, 1.0f, 1.0f,
			T(-0.5f, x, size), T(0, y, size), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y2, light, 1.0f, 1.0f,
		};
	}

	std::vector<unsigned int> planeIndices{
		0, 1, 2,
		2, 3, 0,
	};

	for (int i = 0; i < planeVertices.size(); i++) {
		vertices.push_back(planeVertices[i]);
	}
	for (int i = 0; i < planeIndices.size(); i++) {
		indices.push_back((indices.size() / 6) * 4 + planeIndices[i]);
	}
}

glp::Mesh ChunkGenerator::generateMesh()
{
	auto vertices = std::vector<float>();
	auto indices = std::vector<unsigned int>();
	
	float size = 1;
	float offset = size / 2;

	for (int x = 0; x < this->cells.size(); x++) {
		for (int z = 0; z < this->cells[x].size(); z++) {
			for (int y = 0; y < this->cells[x][z].size(); y++) {
				int block = this->cells[x][z][y];

				float positionX = x * size;
				float positionZ = z * size;
				float positionY = y * size;
				if (block > 0) {
					Block coordinates = GetBlock((BLOCK)block);

					if (z == this->chunkWidth - 1 || this->cells[x][z + 1][y] == 0) {
						createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ + offset, size, coordinates.side, 0.6);
					}
					if (z == 0 || this->cells[x][z - 1][y] == 0) {
						createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ - offset, size, coordinates.side, 0.6);
					}

					if (x == this->chunkWidth - 1 || this->cells[x + 1][z][y] == 0) {
						createPlane(PlaneType::VERTICALZ, vertices, indices, positionX + offset, positionY, positionZ, size, coordinates.side, 0.8);
					}
					if (x == 0 || this->cells[x - 1][z][y] == 0) {
						createPlane(PlaneType::VERTICALZ, vertices, indices, positionX - offset, positionY, positionZ, size, coordinates.side, 0.8);
					}
					
					if (y == this->maxHeight - 1 || this->cells[x][z][y + 1] == 0) {
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + offset, positionZ, size, coordinates.top, 1);
					}
					if (y == 0 || this->cells[x][z][y - 1] == 0) {
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY - offset, positionZ, size, coordinates.bottom, 0.6);
					}
				}
			}
		}
	}

	this->status = ChunkStatus::MESH_GENERATED;
	return glp::Mesh(glp::Mesh::DefaultVertexLayout, vertices, indices);
}

Block ChunkGenerator::GetBlock(BLOCK block) {
	
	switch (block) {
	case GRASS:
		Block block;
		block.top = this->textureAtlas.getTextureCoordinates(0, 0);
		block.side = this->textureAtlas.getTextureCoordinates(1, 0);
		block.bottom = this->textureAtlas.getTextureCoordinates(0, 1);
		return block;
	case DIRT:
		block;
		block.top = this->textureAtlas.getTextureCoordinates(0, 1);
		block.side = this->textureAtlas.getTextureCoordinates(0, 1);
		block.bottom = this->textureAtlas.getTextureCoordinates(0, 1);
		return block;
	default:
		block;
		block.top = this->textureAtlas.getTextureCoordinates(0, 0);
		block.side = this->textureAtlas.getTextureCoordinates(0, 0);
		block.bottom = this->textureAtlas.getTextureCoordinates(0, 0);
		return block;
	}
	//return Block();
}
