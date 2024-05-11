#include "ChunkGenerator.h"
#include "ChunkManager.h"

ChunkGenerator::ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, float detailMultiplier, TextureAtlas& textureAtlas, std::unordered_map<int64_t, ChunkGenerator*>& chunksList, std::shared_mutex& chunkListLock) :
	chunksList(chunksList),
	chunkListLock(chunkListLock),
	textureAtlas(textureAtlas) {
	this->chunkWidth = chunkWidth;
	this->maxHeight = maxHeight;
	this->chunkX = chunkX;
	this->chunkZ = chunkZ;
	this->cells = {};
	this->detailMultiplier = detailMultiplier;
	this->mesh = nullptr;
	this->chunkEntity = NULL;

	this->decorations = {};
	this->overflowLeft = {};
	this->overflowRight = {};
	this->overflowBefore = {};
	this->overflowAfter = {};

	this->status = ChunkStatus::NONE;
}

ChunkGenerator::~ChunkGenerator()
{
	delete this->mesh;
	if (this->chunkEntity != nullptr) {
		delete this->chunkEntity->model;
		this->chunkEntity->model = nullptr;
		delete this->chunkEntity;
		this->chunkEntity = nullptr;
	}
}

float NoiseStandard(float x, float y, float multiplier, int octaves, float gain, float lacunarity, float weight, FastNoiseLite noise) {
	noise.SetFractalOctaves(octaves);
	noise.SetFractalGain(gain);
	noise.SetFractalWeightedStrength(weight);
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
	float value = 0;

	noise.SetSeed(100);
	float defaultNoise = NoiseStandard(x, z, 0.5, 4, 0.5, 2, 0.5, noise);
	defaultNoise = (defaultNoise + 1) / 2;

	noise.SetSeed(10);
	float elevation = NoiseStandard(x, z, 0.04, 4, 0.5, 2, 0.5, noise);
	elevation = (elevation + 1) / 2;

	noise.SetSeed(123321);
	float erosion = NoiseStandard(x, z, 0.06, 3, 0.5, 2, 0.5, noise) - 0;
	erosion = (erosion + 1) / 2;

	float threshold = 0.7; // Set threshold for when erosion effects become significant
	float sharpness = 10.0; // Controls how sharp the transition is at the threshold
	erosion = 1.0 / (1.0 + exp(-sharpness * (erosion - threshold)));



	float d = 3;
	float k = 160;

	noise.SetSeed(1);
	float mountains = NoiseStandard(x, z, 0.25, 1, 0.5, 2, 0, noise);
	float noiseRight = NoiseStandard(x + 1, z, 0.25, 1, 0.5, 2, 0, noise);
	float noiseForward = NoiseStandard(x, z + 1, 0.25, 1, 0.5, 2, 0, noise);

	float slopeX = (noiseRight - mountains) / d;
	float slopeY = (noiseForward - mountains) / d;
	float slope = std::sqrt(slopeX * slopeX + slopeY * slopeY);

	mountains = glm::mix(0.0f, mountains, 1.0f / (1.0f + k * slope));
	mountains = (mountains + 1) / 2;


	noise.SetSeed(2);
	float mountains2 = NoiseStandard(x, z, 0.25 * 2, 1, 0.5, 2, 0, noise);
	float noiseRight2 = NoiseStandard(x + 1, z, 0.25 * 2, 1, 0.5, 2, 0, noise);
	float noiseForward2 = NoiseStandard(x, z + 1, 0.25 * 2, 1, 0.5, 2, 0, noise);

	float slopeX2 = (noiseRight2 - mountains2) / d;
	float slopeY2 = (noiseForward2 - mountains2) / d;
	float slope2 = std::sqrt(slopeX2 * slopeX2 + slopeY2 * slopeY2) + slope;

	mountains2 = glm::mix(0.0f, mountains2, 1.0f / (1.0f + k * slope2));
	mountains2 = (mountains2 + 1) / 2;

	mountains = glm::mix(mountains, mountains2, 0.25f);


	noise.SetSeed(3);
	float mountains3 = NoiseStandard(x, z, 0.25 * 4, 1, 0.5, 2, 0, noise);
	float noiseRight3 = NoiseStandard(x + 1, z, 0.25 * 4, 1, 0.5, 2, 0, noise);
	float noiseForward3 = NoiseStandard(x, z + 1, 0.25 * 4, 1, 0.5, 2, 0, noise);

	float slopeX3 = (noiseRight3 - mountains3) / d;
	float slopeY3 = (noiseForward3 - mountains3) / d;
	float slope3 = std::sqrt(slopeX3 * slopeX3 + slopeY3 * slopeY3) + slope + slope2;

	mountains3 = glm::mix(0.0f, mountains3, 1.0f / (1.0f + k * slope3));
	mountains3 = (mountains3 + 1) / 2;

	mountains = glm::mix(mountains, mountains3, 0.25f / 1);



	noise.SetSeed(4);
	float mountains4 = NoiseStandard(x, z, 0.25 * 8, 1, 0.5, 2, 0, noise);
	float noiseRight4 = NoiseStandard(x + 1, z, 0.25 * 8, 1, 0.5, 2, 0, noise);
	float noiseForward4 = NoiseStandard(x, z + 1, 0.25 * 8, 1, 0.5, 2, 0, noise);

	float slopeX4 = (noiseRight4 - mountains4) / d;
	float slopeY4 = (noiseForward4 - mountains4) / d;
	float slope4 = std::sqrt(slopeX4 * slopeX4 + slopeY4 * slopeY4) + slope + slope2 + slope3;

	mountains4 = glm::mix(0.0f, mountains4, 1.0f / (1.0f + k * slope4));
	mountains4 = (mountains4 + 1) / 2;

	mountains = glm::mix(mountains, mountains4, 0.25f / 1);
	mountains *= 1.2;


	elevation = glm::mix(defaultNoise, elevation, 0.9);
	elevation = glm::mix(0.0f, 0.6f, elevation);
	mountains = glm::mix(0.0f, pow(1.0f - elevation, 2), mountains);
	float sharperErosion = 1.0f / (1.0f + exp(-10.0f * (erosion - 0.5f)));
	mountains *= sharperErosion;
	value = elevation + mountains;

	value *= this->maxHeight /* this->detailMultiplier*/;
	value += 1;

	return value;
}

void ChunkGenerator::generateTerain() {
	FastNoiseLite noise(1);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

	int waterLevel = this->maxHeight / 3;

	this->cells.clear();
	this->decorations.clear();
	this->overflowLeft.clear();
	this->overflowRight.clear();
	this->overflowBefore.clear();
	this->overflowAfter.clear();

	this->cells.resize(this->chunkWidth * this->chunkWidth * this->maxHeight * this->detailMultiplier * this->detailMultiplier);
	for (int x = 0; x < this->chunkWidth * detailMultiplier; x++) {
		for (int z = 0; z < this->chunkWidth * detailMultiplier; z++) {
			float xValue = ((x / this->detailMultiplier) + this->chunkX * this->chunkWidth);
			float zValue = ((z / this->detailMultiplier) + this->chunkZ * this->chunkWidth);

			int terainHeight = this->GetTerainHeight(xValue, zValue, noise);

			for (int y = 0; y < this->maxHeight; y++) {
				/*noise.SetSeed(3458976);
				noise.SetFractalOctaves(4);
				float air = noise.GetNoise(xValue * 2, zValue * 2, (float)y) ;*/

				int block = BLOCK::Air;
				if (y <= terainHeight) {
					block = BLOCK::GRASS;
					if (y <= waterLevel + 2) {
						block = BLOCK::SAND;
					}
					else if (y > this->maxHeight - (this->maxHeight / 5) * 2) {
						block = BLOCK::SNOW;
					}
				}
				else if (y <= waterLevel) {
					block = BLOCK::WATER;
				}
				this->cells[this->getIndex(x, z, y)] = block;
			}
		}
	}

	for (int x = 0; x < this->chunkWidth * detailMultiplier; x++) {
		for (int z = 0; z < this->chunkWidth * detailMultiplier; z++) {
			float xValue = (x / this->detailMultiplier + this->chunkX * this->chunkWidth);
			float zValue = (z / this->detailMultiplier + this->chunkZ * this->chunkWidth);
			float treePlacement = NoiseStandard(xValue, zValue, 50, 1, 1, 1, 0, noise);
			if (treePlacement > 0.98 - glm::pow(1 - this->detailMultiplier, 8)) {
				treePlacement = 1;
			}
			else {
				treePlacement = 0;
			}
			for (int y = 0; y < this->maxHeight - 10; y++) {

				if (treePlacement == 1 && this->cells[this->getIndex(x, z, y)] == BLOCK::GRASS && this->cells[this->getIndex(x, z, y + 1)] == BLOCK::Air) {

					float height = 8;
					float trunkSize = 1 * this->detailMultiplier;
					float sphereWidth = 8 * this->detailMultiplier;
					float sphereHeight = 8;

					for (int i = -trunkSize / 2; i < trunkSize / 2; i++) {
						for (int j = -trunkSize / 2; j < trunkSize / 2; j++) {
							for (int k = 0; k < height; k++) {
								if (glm::sqrt(i * i + j * j) < trunkSize / 2) {
									this->addCell(x + i, z + j, y + 1 + k, BLOCK::WOOD);
								}
								
							}
						}
					}
					for (float i = -sphereWidth / 2; i < sphereWidth / 2; i++) {
						for (float j = -sphereWidth / 2; j < sphereWidth / 2; j++) {
							for (float k = 0; k < sphereHeight / 2; k++) {
								float k2 = k * this->detailMultiplier;
								if (glm::sqrt(i * i + j * j + k2 * k2) < sphereWidth / 2) {
									this->addCell(x + i, z + j, y + k + 1 + height + sphereHeight / 2 - 1 - 5, BLOCK::LEAF);
								}
							}
						}
					}

					break;
				}

			}
		}
	}

	this->status = ChunkStatus::TERAIN_GENERATED;
}

void ChunkGenerator::generateDecorations() {
	for (auto pos : this->decorations) {
		this->cells[this->getIndex(pos.x, pos.z, pos.y)] = pos.w;
	}

	auto keyLeft = ChunkManager::getKey(this->chunkX + 1, this->chunkZ);
	if (this->chunksList.find(keyLeft) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyLeft);
		for (auto pos : chunk->overflowLeft) {
			if (pos.x >= this->chunkWidth * detailMultiplier || pos.z >= this->chunkWidth * detailMultiplier) continue;
			this->cells[this->getIndex(pos.x, pos.z, pos.y)] = pos.w;
		}
	}
	auto keyRight = ChunkManager::getKey(this->chunkX - 1, this->chunkZ);
	if (this->chunksList.find(keyRight) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyRight);
		for (auto pos : chunk->overflowRight) {
			if (pos.x >= this->chunkWidth * detailMultiplier || pos.z >= this->chunkWidth * detailMultiplier) continue;
			this->cells[this->getIndex(pos.x, pos.z, pos.y)] = pos.w;
		}
	}
	auto keyBefore = ChunkManager::getKey(this->chunkX, this->chunkZ + 1);
	if (this->chunksList.find(keyBefore) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyBefore);
		for (auto pos : chunk->overflowBefore) {
			if (pos.x >= this->chunkWidth * detailMultiplier || pos.z >= this->chunkWidth * detailMultiplier) continue;
			this->cells[this->getIndex(pos.x, pos.z, pos.y)] = pos.w;
		}
	}
	auto keyAfter = ChunkManager::getKey(this->chunkX, this->chunkZ - 1);
	if (this->chunksList.find(keyAfter) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyAfter);
		for (auto pos : chunk->overflowAfter) {
			if (pos.x >= this->chunkWidth * detailMultiplier || pos.z >= this->chunkWidth * detailMultiplier) continue;
			this->cells[this->getIndex(pos.x, pos.z, pos.y)] = pos.w;
		}
	}

	this->status = ChunkStatus::DECORATIONS_GENERATED;
}

void ChunkGenerator::addCell(int x, int z, int y, BLOCK block) {
	glm::vec4 cell = { x, y, z, block };
	if (x < 0) {
		if (z < 0 || z >= this->chunkWidth * this->detailMultiplier) return;
		cell.x = this->chunkWidth * this->detailMultiplier + x;
		this->overflowLeft.push_back(cell);
	}
	else if (x >= this->chunkWidth * this->detailMultiplier) {
		if (z < 0 || z >= this->chunkWidth * this->detailMultiplier) return;
		cell.x = x - this->chunkWidth * this->detailMultiplier;
		this->overflowRight.push_back(cell);
	}
	else if (z < 0) {
		if (x < 0 || x >= this->chunkWidth * this->detailMultiplier) return;
		cell.z = this->chunkWidth * this->detailMultiplier + z;
		this->overflowBefore.push_back(cell);
	}
	else if (z >= this->chunkWidth * this->detailMultiplier) {
		if (x < 0 || x >= this->chunkWidth * this->detailMultiplier) return;
		cell.z = z - this->chunkWidth * this->detailMultiplier;
		this->overflowAfter.push_back(cell);
	}
	else {
		//this->cells[x][z][y] = block;
		this->decorations.push_back(cell);
	}
}

float T(float VertexPosition, float position, float size) {
	return VertexPosition * size + position;
}


enum PlaneType {
	VERTICALX,
	VERTICALZ,
	HORIZONTAL
};
void createPlane(PlaneType planeType, std::vector<float>& vertices, std::vector<unsigned int>& indices, float x, float y, float z, float size, TextureCoordinates textureCoordinates, glm::vec4 light, float faceDirection) {
	float voxelHeight = 1;

	std::vector<float> planeVertices;
	if (planeType == PlaneType::VERTICALX) {
		planeVertices = {
			T(-0.5f, x, size), T(-0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x1, textureCoordinates.y1, light.x, 1.0f, 1.0f, faceDirection,
			T(0.5f, x, size), T(-0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x2, textureCoordinates.y1, light.y, 1.0f, 1.0f, faceDirection,
			T(0.5f, x, size), T(0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x2, textureCoordinates.y2, light.z, 1.0f, 1.0f, faceDirection,
			T(-0.5f, x, size), T(0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x1, textureCoordinates.y2, light.w, 1.0f, 1.0f, faceDirection,
		};
	}
	else if (planeType == PlaneType::VERTICALZ) {
		planeVertices = {
			T(0, x, size), T(-0.5f, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y1, light.x, 1.0f, 1.0f, faceDirection,
			T(0, x, size), T(-0.5f, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y1, light.y, 1.0f, 1.0f, faceDirection,
			T(0, x, size), T(0.5f, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y2, light.z, 1.0f, 1.0f, faceDirection,
			T(0, x, size), T(0.5f, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y2, light.w, 1.0f, 1.0f, faceDirection,
		};
	}
	else if (planeType == PlaneType::HORIZONTAL) {
		planeVertices = {
			T(-0.5f, x, size), T(0, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x1, textureCoordinates.y1, light.x, 1.0f, 1.0f, faceDirection,
			T(0.5f, x, size), T(0, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y1, light.y, 1.0f, 1.0f, faceDirection,
			T(0.5f, x, size), T(0, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x2, textureCoordinates.y2, light.z, 1.0f, 1.0f, faceDirection,
			T(-0.5f, x, size), T(0, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y2, light.w, 1.0f, 1.0f, faceDirection,
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

using namespace std::chrono_literals;
int ChunkGenerator::getBlockValue(int x, int y, int z, std::unordered_map<int64_t, ChunkGenerator*> &closeChunks) {
	float width = this->chunkWidth * this->detailMultiplier;
	if (y < 0 || y > this->maxHeight - 1) {
		return 1;
	}
	else if (x >= 0 && x <= width - 1 && z >= 0 && z <= width - 1) {
		return this->cells[this->getIndex(x, z, y)];
	}
	else if ((x < 0 || x > width - 1) && (z < 0 || z > width - 1)) return 1;

	int chunkX = this->chunkX + glm::floor((float)x / width);
	int chunkZ = this->chunkZ + glm::floor((float)z / width);
	auto chunkKey = ChunkManager::getKey(chunkX, chunkZ);
	int blockValue = 1;
	auto chunk = closeChunks.at(chunkKey);
	x = (int)glm::mod((float)x, width);
	z = (int)glm::mod((float)z, width);
	x = x * chunk->detailMultiplier / this->detailMultiplier;
	z = z * chunk->detailMultiplier / this->detailMultiplier;
	blockValue = chunk->cells[chunk->getIndex(x, z, y)];
	return blockValue;
}

void ChunkGenerator::generateMesh() {
	delete this->mesh;
	this->mesh = nullptr;

	auto vertices = std::vector<float>();
	auto indices = std::vector<unsigned int>();

	std::unordered_map<int64_t, ChunkGenerator*> nearChunks;
	auto keyLeft = ChunkManager::getKey(chunkX - 1, chunkZ);
	if (this->chunksList.find(keyLeft) != this->chunksList.end()) {
		nearChunks.emplace(keyLeft, this->chunksList.at(keyLeft));
	}
	auto keyRight = ChunkManager::getKey(chunkX + 1, chunkZ);
	if (this->chunksList.find(keyRight) != this->chunksList.end()) {
		nearChunks.emplace(keyRight, this->chunksList.at(keyRight));
	}
	auto keyForward = ChunkManager::getKey(chunkX, chunkZ + 1);
	if (this->chunksList.find(keyForward) != this->chunksList.end()) {
		nearChunks.emplace(keyForward, this->chunksList.at(keyForward));
	}
	auto keyBackward = ChunkManager::getKey(chunkX, chunkZ - 1);
	if (this->chunksList.find(keyBackward) != this->chunksList.end()) {
		nearChunks.emplace(keyBackward, this->chunksList.at(keyBackward));
	}
	
	float size = 1 / this->detailMultiplier;
	int width = this->chunkWidth * this->detailMultiplier;
	int height = this->maxHeight;
	float offset = size / 2;
	float positionOffset = 0;
	if (this->detailMultiplier != 1) {
		positionOffset = size / 4;
	}
	if (this->detailMultiplier <= 0.25) {
		positionOffset += 0.5;
	}
	if (this->detailMultiplier <= 0.125) {
		positionOffset += 1;
	}
	if (this->detailMultiplier <= 0.0625) {
		positionOffset += 2;
	}

	float AmbientOcclusionPerVoxel = 0.2;
	for (int i = 0; i < this->cells.size(); i++) {
		int block = this->cells[i];
		if (block != BLOCK::Air) {
			auto position = this->getXYZ(i);
			auto x = position.x;
			auto y = position.y;
			auto z = position.z;

			float positionX = x * size + positionOffset;
			float positionZ = z * size + positionOffset;
			float positionY = y;
			if (this->detailMultiplier != 1) {
				positionY -= size;
			}
			//left up right down front back

			Block coordinates = GetBlock((BLOCK)block);
			if (block == BLOCK::WATER) {
				if (this->getBlockValue(x, y + 1, z, nearChunks) != BLOCK::Air) continue;
				createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + 0.5, positionZ, size, coordinates.top, { 1, 1, 1, 1 }, 1);
				continue;
			}

			auto blockBefore = this->getBlockValue(x, y, z + 1, nearChunks);
			auto blockAfter = this->getBlockValue(x, y, z - 1, nearChunks);
			auto blockRight = this->getBlockValue(x + 1, y, z, nearChunks);
			auto blockLeft = this->getBlockValue(x - 1, y, z, nearChunks);
			auto blockUp = this->getBlockValue(x, y + 1, z, nearChunks);
			auto blockDown = this->getBlockValue(x, y - 1, z, nearChunks);

			auto blockAfterLeft = 1;
			auto blockAfterRight = 1;
			if (blockAfter == BLOCK::Air || blockAfter == BLOCK::WATER || blockLeft == BLOCK::Air || blockLeft == BLOCK::WATER) {
				blockAfterLeft = this->getBlockValue(x - 1, y, z - 1, nearChunks);
			}
			if (blockAfter == BLOCK::Air || blockAfter == BLOCK::WATER || blockRight == BLOCK::Air || blockRight == BLOCK::WATER) {
				blockAfterRight = this->getBlockValue(x + 1, y, z - 1, nearChunks);
			}

			auto blockBeforeLeft = 1;
			auto blockBeforeRight = 1;
			if (blockBefore == BLOCK::Air || blockBefore == BLOCK::WATER || blockLeft == BLOCK::Air || blockLeft == BLOCK::WATER) {
				blockBeforeLeft = this->getBlockValue(x - 1, y, z + 1, nearChunks);
			}
			if (blockBefore == BLOCK::Air || blockBefore == BLOCK::WATER || blockRight == BLOCK::Air || blockRight == BLOCK::WATER) {
				blockBeforeRight = this->getBlockValue(x + 1, y, z + 1, nearChunks);
			}

			if (blockBefore == BLOCK::Air || blockBefore == BLOCK::WATER) {
				auto blockBeforeDown = this->getBlockValue(x, y - 1, z + 1, nearChunks);

				glm::vec4 light = { 0.6f, 0.6f, 0.6f, 0.6f };
				if (blockBeforeDown != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
					light[1] -= AmbientOcclusionPerVoxel;
				}
				if (blockBeforeLeft != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;;
				}
				if (blockBeforeRight != BLOCK::Air) {
					light[1] -= AmbientOcclusionPerVoxel;;
				}
				createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ + offset, size, coordinates.side, light, 5);
			}
			
			if (blockAfter == BLOCK::Air || blockAfter == BLOCK::WATER) {
				auto blockAfterDown = this->getBlockValue(x, y - 1, z - 1, nearChunks);

				glm::vec4 light = { 0.8f, 0.8f, 0.8f, 0.8f };
				if (blockAfterDown != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
					light[1] -= AmbientOcclusionPerVoxel;
				}
				if (blockAfterLeft != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;;
				}
				if (blockAfterRight != BLOCK::Air) {
					light[1] -= AmbientOcclusionPerVoxel;;
				}
				createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ - offset, size, coordinates.side, light, 4);
			}

			
			if (blockRight == BLOCK::Air || blockRight == BLOCK::WATER) {
				auto blockRightDown = this->getBlockValue(x + 1, y - 1, z, nearChunks);

				glm::vec4 light = { 0.8f, 0.8f, 0.8f, 0.8f };
				if (blockRightDown != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
					light[1] -= AmbientOcclusionPerVoxel;
				}
				if (blockBeforeRight != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
				}
				if (blockAfterRight != BLOCK::Air) {
					light[1] -= AmbientOcclusionPerVoxel;
				}
				createPlane(PlaneType::VERTICALZ, vertices, indices, positionX + offset, positionY, positionZ, size, coordinates.side, light, 2);
			}
			
			if (blockLeft == BLOCK::Air || blockLeft == BLOCK::WATER) {
				auto blockLeftDown = this->getBlockValue(x - 1, y - 1, z, nearChunks);

				glm::vec4 light = { 0.6f, 0.6f, 0.6f, 0.6f };
				if (blockLeftDown != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
					light[1] -= AmbientOcclusionPerVoxel;
				}
				if (blockBeforeLeft != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
				}
				if (blockAfterLeft != BLOCK::Air) {
					light[1] -= AmbientOcclusionPerVoxel;
				}
				createPlane(PlaneType::VERTICALZ, vertices, indices, positionX - offset, positionY, positionZ, size, coordinates.side, light, 0);
			}

			
			if (blockUp == BLOCK::Air || blockUp == BLOCK::WATER) {
				auto blockBeforeUp = this->getBlockValue(x, y + 1, z + 1, nearChunks);
				auto blockAfterUp = this->getBlockValue(x, y + 1, z - 1, nearChunks);
				auto blockLeftUp = this->getBlockValue(x - 1, y + 1, z, nearChunks);
				auto blockRightUp = this->getBlockValue(x + 1, y + 1, z, nearChunks);

				auto blockAfterLeftUp = this->getBlockValue(x - 1, y + 1, z - 1, nearChunks);
				auto blockAfterRightUp = this->getBlockValue(x + 1, y + 1, z - 1, nearChunks);
				auto blockBeforeLeftUp = this->getBlockValue(x - 1, y + 1, z + 1, nearChunks);
				auto blockBeforeRightUp = this->getBlockValue(x + 1, y + 1, z + 1, nearChunks);

				glm::vec4 light = { 1, 1, 1, 1 };
				if (blockAfterUp != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
					light[1] -= AmbientOcclusionPerVoxel;
				}
				if (blockBeforeUp != BLOCK::Air) {
					light[2] -= AmbientOcclusionPerVoxel;
					light[3] -= AmbientOcclusionPerVoxel;
				}
				if (blockLeftUp != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
					light[3] -= AmbientOcclusionPerVoxel;
				}
				if (blockRightUp != BLOCK::Air) {
					light[1] -= AmbientOcclusionPerVoxel;
					light[2] -= AmbientOcclusionPerVoxel;
				}

				if (blockAfterLeftUp != BLOCK::Air) {
					light[0] -= AmbientOcclusionPerVoxel;
				}
				if (blockAfterRightUp != BLOCK::Air) {
					light[1] -= AmbientOcclusionPerVoxel;
				}

				if (blockBeforeLeftUp != BLOCK::Air) {
					light[3] -= AmbientOcclusionPerVoxel;
				}
				if (blockBeforeRightUp != BLOCK::Air) {
					light[2] -= AmbientOcclusionPerVoxel;
				}


				createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + 0.5, positionZ, size, coordinates.top, light, 1); // front-left front-right back.right back-left
			}
			
			if (blockDown == BLOCK::Air || blockDown == BLOCK::WATER) {
				createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY - 0.5, positionZ, size, coordinates.bottom, { 0.6, 0.6, 0.6, 0.6 }, 3);
			}
		}
	}

	this->mesh = new glp::Mesh({ 3, 2, 3, 1 }, vertices, indices);
	this->status = ChunkStatus::MESH_GENERATED;
}

Block ChunkGenerator::GetBlock(BLOCK block) {
	
	switch (block) {
		case GRASS:
			Block block;
			block.top = this->textureAtlas.getTextureCoordinates(0, 0);
			block.side = this->textureAtlas.getTextureCoordinates(0, 0);
			block.bottom = this->textureAtlas.getTextureCoordinates(0, 1);
			return block;
		case DIRT:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(0, 1);
			block.side = this->textureAtlas.getTextureCoordinates(0, 1);
			block.bottom = this->textureAtlas.getTextureCoordinates(0, 1);
			return block;
		case SAND:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(3, 0);
			block.side = this->textureAtlas.getTextureCoordinates(3, 0);
			block.bottom = this->textureAtlas.getTextureCoordinates(3, 0);
			return block;
		case WATER:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(1, 2);
			block.side = this->textureAtlas.getTextureCoordinates(1, 2);
			block.bottom = this->textureAtlas.getTextureCoordinates(1, 2);
			return block;
		case WOOD:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(1, 1);
			block.side = this->textureAtlas.getTextureCoordinates(1, 1);
			block.bottom = this->textureAtlas.getTextureCoordinates(1, 1);
			return block;
		case LEAF:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(2, 1);
			block.side = this->textureAtlas.getTextureCoordinates(2, 1);
			block.bottom = this->textureAtlas.getTextureCoordinates(2, 1);
			return block;
		case SNOW:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(3, 1);
			block.side = this->textureAtlas.getTextureCoordinates(3, 1);
			block.bottom = this->textureAtlas.getTextureCoordinates(3, 1);
			return block;
		default:
			block;
			block.top = this->textureAtlas.getTextureCoordinates(0, 0);
			block.side = this->textureAtlas.getTextureCoordinates(0, 0);
			block.bottom = this->textureAtlas.getTextureCoordinates(0, 0);
			return block;
	}
}
