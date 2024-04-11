#include "ChunkGenerator.h"

ChunkGenerator::ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, float detailMultiplier, TextureAtlas& textureAtlas, std::unordered_map<std::string, ChunkGenerator*>& chunksList) :
	chunksList(chunksList),
	textureAtlas(textureAtlas) {
	this->chunkWidth = chunkWidth;
	this->maxHeight = maxHeight;
	this->chunkX = chunkX;
	this->chunkZ = chunkZ;
	this->cells = {};
	this->detailMultiplier = detailMultiplier;
	this->mesh = nullptr;
	this->chunkEntity = NULL;

	this->status = ChunkStatus::NONE;
	this->generateTerain();
}

ChunkGenerator::~ChunkGenerator()
{
	std::cout << "delete chunk destructor " << std::endl;
	delete this->mesh;
}

float NoiseStandard(float x, float y, float multiplier, int octaves, float gain, float lacunarity, FastNoiseLite noise) {
	noise.SetFractalOctaves(octaves);
	noise.SetFractalGain(gain);
	noise.SetFractalLacunarity(lacunarity);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	return noise.GetNoise(x * multiplier, y * multiplier);
}

float NoiseRidged(float x, float y, float multiplier, int octaves, float gain, float lacunarity, FastNoiseLite noise) {
	noise.SetFractalOctaves(octaves);
	noise.SetFractalGain(gain);
	noise.SetFractalLacunarity(lacunarity);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_Ridged);
	return noise.GetNoise(x * multiplier, y * multiplier);
}

float NoisePingPong(float x, float y, float multiplier, int octaves, float gain, float lacunarity, float strength, FastNoiseLite noise) {
	noise.SetFractalOctaves(octaves);
	noise.SetFractalGain(gain);
	noise.SetFractalLacunarity(lacunarity);
	noise.SetFractalPingPongStrength(strength);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_PingPong);
	return noise.GetNoise(x * multiplier, y * multiplier);
}

int ChunkGenerator::GetTerainHeight(float x, float z, FastNoiseLite noise) {
	float xValue = ((x / this->detailMultiplier) + this->chunkX * this->chunkWidth);
	float zValue = ((z / this->detailMultiplier) + this->chunkZ * this->chunkWidth);
	float noiseMultiplier = noise.GetNoise(xValue * 0.2, zValue * 0.2);
	noiseMultiplier = (noiseMultiplier + 1) / 2;
	noiseMultiplier = 1 - noiseMultiplier;
	noiseMultiplier *= 0.8;

	float value = NoiseRidged(xValue, zValue, 0.2, 4, 0.5, 2, noise);
	value += NoiseStandard(xValue, zValue, 0.4, 4, noiseMultiplier, 2, noise);

	value += noiseMultiplier * 8;

	value += glm::min(NoisePingPong(xValue * 4, zValue * 4, 0.2, 1, 0.5, 2, 1, noise), 0.5f) * 0.5;

	value /= 3;

	value = (value + 1) / 2;
	value *= 0.4;

	value *= this->maxHeight * this->detailMultiplier;
	value += 1;

	return value;
}

void ChunkGenerator::generateTerain() {
	// Create and configure FastNoise object
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);


	this->cells.clear();
	for (int x = 0; x < this->chunkWidth * detailMultiplier; x++) {
		this->cells.push_back({});
		for (int z = 0; z < this->chunkWidth * detailMultiplier; z++) {
			int terainHeight = this->GetTerainHeight((float)x, (float)z, noise);
			this->cells[x].push_back({});
			for (int y = 0; y < this->maxHeight * detailMultiplier; y++) {
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

int ChunkGenerator::getBlockValue(int x, int y, int z) {
	/*x = x * this->detailMultiplier;
	y = y * this->detailMultiplier;
	z = z * this->detailMultiplier;*/
	int oldX = x;
	int oldY = y;
	int oldZ = z;
	float width = this->chunkWidth * this->detailMultiplier;
	float height = this->maxHeight * this->detailMultiplier;

	if (y < 0 || y > height - 1) {
		return 1;
	}
	if (x >= 0 && x <= width - 1 && z >= 0 && z <= width - 1) {
		return this->cells[x][z][y];
	}

	int chunkX = this->chunkX + glm::floor((float)x / width);
	int chunkZ = this->chunkZ + glm::floor((float)z / width);
	
	x = (int)glm::mod((float)x, width);
	z = (int)glm::mod((float)z, width);

	std::string chunkKey = std::to_string(chunkX) + "|" + std::to_string(chunkZ);
	if (this->chunksList.find(chunkKey) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(chunkKey);
		x = x * chunk->detailMultiplier / this->detailMultiplier;
		z = z * chunk->detailMultiplier / this->detailMultiplier;
		y = y * chunk->detailMultiplier / this->detailMultiplier;
		int blockValue = chunk->cells[x][z][y];
		return blockValue;
	}

	return 0;
}

void ChunkGenerator::generateMesh() {
	delete this->mesh;
	this->mesh = nullptr;

	auto vertices = std::vector<float>();
	auto indices = std::vector<unsigned int>();

	float size = 1 / this->detailMultiplier;
	int width = this->chunkWidth * this->detailMultiplier;
	int height = this->maxHeight * this->detailMultiplier;
	float offset = size / 2;
	float positionOffset = 0;
	if (this->detailMultiplier != 1) {
		positionOffset = size / 4;
	}

	for (int x = 0; x < this->cells.size(); x++) {
		for (int z = 0; z < this->cells[x].size(); z++) {
			for (int y = 0; y < this->cells[x][z].size(); y++) {
				int block = this->cells[x][z][y];

				float positionX = x * size + positionOffset;
				float positionZ = z * size + positionOffset;
				float positionY = y * size + positionOffset - (1 / this->detailMultiplier * 2);
				if (block > 0) {
					Block coordinates = GetBlock((BLOCK)block);

					if (this->getBlockValue(x, y, z + 1) == 0) {
						createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ + offset, size, coordinates.side, 0.6);
					}
					if (this->getBlockValue(x, y, z - 1) == 0) {
						createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ - offset, size, coordinates.side, 0.6);
					}

					if (this->getBlockValue(x + 1, y, z) == 0) {
						createPlane(PlaneType::VERTICALZ, vertices, indices, positionX + offset, positionY, positionZ, size, coordinates.side, 0.8);
					}
					if (this->getBlockValue(x - 1, y, z) == 0) {
						createPlane(PlaneType::VERTICALZ, vertices, indices, positionX - offset, positionY, positionZ, size, coordinates.side, 0.8);
					}
					
					if (this->getBlockValue(x, y + 1, z) == 0) {
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + offset, positionZ, size, coordinates.top, 1);
					}
					if (this->getBlockValue(x, y - 1, z) == 0) {
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY - offset, positionZ, size, coordinates.bottom, 0.6);
					}
				}
			}
		}
	}

	this->status = ChunkStatus::MESH_GENERATED;
	//std::cout << "generateMesh" << std::endl;
	this->mesh = new glp::Mesh(glp::Mesh::DefaultVertexLayout, vertices, indices);
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
