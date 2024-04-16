#include "ChunkGenerator.h"

ChunkGenerator::ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, float detailMultiplier, TextureAtlas& textureAtlas, std::unordered_map<std::string, ChunkGenerator*>& chunksList, std::shared_mutex& chunkListLock) :
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
	//std::cout << "delete chunk destructor " << std::endl;
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
	float xValue = ((x / this->detailMultiplier) + this->chunkX * this->chunkWidth);
	float zValue = ((z / this->detailMultiplier) + this->chunkZ * this->chunkWidth);
	float noiseMultiplier = noise.GetNoise(xValue * 0.08, zValue * 0.08);
	/*noiseMultiplier = (noiseMultiplier + 1) / 2;
	noiseMultiplier = 1 - noiseMultiplier;
	noiseMultiplier *= 1.5;*/

	float value = 0.5 + (noiseMultiplier / 2 + 0.5) * 1.4;

	value += NoiseStandard(xValue, zValue, 0.2, 4, 0.5, 2, 0.7 - noiseMultiplier / 2, noise) - 0.5;

	/*float value = glm::pow(NoiseRidged(xValue, zValue, 0.1, 4, 0.5, 2, noise), 2);

	value *= noiseMultiplier;
	value += noiseMultiplier * 1;

	value -= glm::clamp<float>(glm::min(NoisePingPong(xValue * 0.8, zValue * 0.8, 0.2, 3, 0.5, 2, 1, noise), 0.5f) * 0.8f, 0, 0.5f);

	value += NoiseStandard(xValue, zValue, 0.8, 4, 0.5, 2, noise) / 2 - 0.5;*/

	//value /= 3;

	value = (value + 1) / 2;
	value *= 0.5;

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
	for (int x = 0; x < this->chunkWidth * detailMultiplier; x++) {
		this->cells.push_back({});
		for (int z = 0; z < this->chunkWidth * detailMultiplier; z++) {
			int terainHeight = this->GetTerainHeight((float)x, (float)z, noise);
			this->cells[x].push_back({});
			for (int y = 0; y < this->maxHeight; y++) {
				int block = BLOCK::Air;
				if (y <= terainHeight) {
					block = BLOCK::GRASS;
					if (y <= waterLevel + 2) {
						block = BLOCK::SAND;
					}
				}
				else if (y <= waterLevel) {
					block = BLOCK::WATER;
				}
				this->cells[x][z].push_back(block);
			}
		}
	}

	for (int x = 0; x < this->chunkWidth * detailMultiplier; x++) {
		for (int z = 0; z < this->chunkWidth * detailMultiplier; z++) {
			float xValue = (x / this->detailMultiplier + this->chunkX * this->chunkWidth);
			float zValue = (z / this->detailMultiplier + this->chunkZ * this->chunkWidth);
			float treePlacement = NoiseStandard(xValue, zValue, 50, 1, 1, 1, 0, noise);
			if (treePlacement > 0.96) {
				treePlacement = 1;
			}
			else {
				treePlacement = 0;
			}
			for (int y = 0; y < this->maxHeight - 10; y++) {

				if (treePlacement == 1 /*x == (this->chunkWidth * detailMultiplier) / 2 && z == (this->chunkWidth * detailMultiplier) / 2*/ && this->cells[x][z][y] == BLOCK::GRASS && this->cells[x][z][y + 1] == BLOCK::Air) {

					float height = 7;
					float trunkSize = 1;
					float sphereWidth = 6 * this->detailMultiplier;
					float sphereHeight = 6;

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
							for (float k = -sphereHeight / 2; k < sphereHeight / 2; k++) {
								float k2 = k * this->detailMultiplier;
								if (glm::sqrt(i * i + j * j + k2 * k2) < sphereWidth / 2 /*+ glm::min(k, 0)*/) {
									this->addCell(x + i, z + j, y + k + 1 + height + sphereHeight / 2 - 1 - 2, BLOCK::Leaf);
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

void ChunkGenerator::generateDecorations()
{
	for (auto pos : this->decorations) {
		this->cells[pos.x][pos.z][pos.y] = pos.w;
	}

	/*std::string keyLeft = std::to_string((int)this->chunkX + 1) + "|" + std::to_string((int)this->chunkZ);
	if (this->chunksList.find(keyLeft) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyLeft);
		for (auto pos : chunk->overflowLeft) {
			this->cells[pos.x / chunk->detailMultiplier * this->detailMultiplier][pos.z / chunk->detailMultiplier * this->detailMultiplier][pos.y] = pos.w;
		}
	}
	std::string keyRight = std::to_string((int)this->chunkX - 1) + "|" + std::to_string((int)this->chunkZ);
	if (this->chunksList.find(keyRight) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyRight);
		for (auto pos : chunk->overflowRight) {
			this->cells[pos.x / chunk->detailMultiplier * this->detailMultiplier][pos.z / chunk->detailMultiplier * this->detailMultiplier][pos.y] = pos.w;
		}
	}
	std::string keyBefore = std::to_string((int)this->chunkX) + "|" + std::to_string((int)this->chunkZ + 1);
	if (this->chunksList.find(keyBefore) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyBefore);
		for (auto pos : chunk->overflowBefore) {
			this->cells[pos.x / chunk->detailMultiplier * this->detailMultiplier][pos.z / chunk->detailMultiplier * this->detailMultiplier][pos.y] = pos.w;
		}
	}
	std::string keyAfter = std::to_string((int)this->chunkX) + "|" + std::to_string((int)this->chunkZ - 1);
	if (this->chunksList.find(keyAfter) != this->chunksList.end()) {
		auto chunk = this->chunksList.at(keyAfter);
		for (auto pos : chunk->overflowAfter) {
			this->cells[pos.x / chunk->detailMultiplier * this->detailMultiplier][pos.z / chunk->detailMultiplier * this->detailMultiplier][pos.y] = pos.w;
		}
	}*/

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
void createPlane(PlaneType planeType, std::vector<float>& vertices, std::vector<unsigned int>& indices, float x, float y, float z, float size, TextureCoordinates textureCoordinates, glm::vec4 light) {
	float voxelHeight = 1;

	std::vector<float> planeVertices;
	if (planeType == PlaneType::VERTICALX) {
		planeVertices = {
			T(-0.5f, x, size), T(-0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x1, textureCoordinates.y1, light.x, 1.0f, 1.0f,
			T(0.5f, x, size), T(-0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x2, textureCoordinates.y1, light.y, 1.0f, 1.0f,
			T(0.5f, x, size), T(0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x2, textureCoordinates.y2, light.z, 1.0f, 1.0f,
			T(-0.5f, x, size), T(0.5f, y, voxelHeight), T(0.0f, z, size), textureCoordinates.x1, textureCoordinates.y2, light.w, 1.0f, 1.0f,
		};
	}
	else if (planeType == PlaneType::VERTICALZ) {
		planeVertices = {
			T(0, x, size), T(-0.5f, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y1, light.x, 1.0f, 1.0f,
			T(0, x, size), T(-0.5f, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y1, light.y, 1.0f, 1.0f,
			T(0, x, size), T(0.5f, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y2, light.z, 1.0f, 1.0f,
			T(0, x, size), T(0.5f, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y2, light.w, 1.0f, 1.0f,
		};
	}
	else if (planeType == PlaneType::HORIZONTAL) {
		planeVertices = {
			T(-0.5f, x, size), T(0, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x1, textureCoordinates.y1, light.x, 1.0f, 1.0f,
			T(0.5f, x, size), T(0, y, voxelHeight), T(-0.5f, z, size), textureCoordinates.x2, textureCoordinates.y1, light.y, 1.0f, 1.0f,
			T(0.5f, x, size), T(0, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x2, textureCoordinates.y2, light.z, 1.0f, 1.0f,
			T(-0.5f, x, size), T(0, y, voxelHeight), T(0.5f, z, size), textureCoordinates.x1, textureCoordinates.y2, light.w, 1.0f, 1.0f,
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
int ChunkGenerator::getBlockValue(int x, int y, int z) {
	int oldX = x;
	int oldY = y;
	int oldZ = z;
	float width = this->chunkWidth * this->detailMultiplier;
	float height = this->maxHeight; // * this->detailMultiplier;

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
		int blockValue = 1;
		auto chunk = this->chunksList.at(chunkKey);
		if (chunk == nullptr || chunk->status == ChunkStatus::NONE) return 1;
		//std::shared_lock<std::shared_mutex> lock(chunk->chunkLock, std::defer_lock);
		//lock.lock();
		//if (lock.try_lock()) {
			x = x * chunk->detailMultiplier / this->detailMultiplier;
			z = z * chunk->detailMultiplier / this->detailMultiplier;
			//y = y * chunk->detailMultiplier / this->detailMultiplier;
			blockValue = chunk->cells[x][z][y];

			//lock.unlock();
		//}

		return blockValue;
	}

	return 1;
}

void ChunkGenerator::generateMesh() {
	delete this->mesh;
	this->mesh = nullptr;

	auto vertices = std::vector<float>();
	auto indices = std::vector<unsigned int>();

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

	for (int x = 0; x < this->cells.size(); x++) {
		for (int z = 0; z < this->cells[x].size(); z++) {
			for (int y = 0; y < this->cells[x][z].size(); y++) {
				int block = this->cells[x][z][y];

				float positionX = x * size + positionOffset;
				float positionZ = z * size + positionOffset;
				float positionY = y; // *size + positionOffset - (1 / this->detailMultiplier * 2);
				if (this->detailMultiplier != 1) {
					positionY -= size;
				}
				if (block != BLOCK::Air) {
					Block coordinates = GetBlock((BLOCK)block);
					if (block == BLOCK::WATER) {
						if (this->getBlockValue(x, y + 1, z) != BLOCK::Air) continue;
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + 0.5, positionZ, size, coordinates.top, {1, 1, 1, 1});
						continue;
					}

					auto blockBeforeUp = this->getBlockValue(x, y + 1, z + 1);
					auto blockAfterUp = this->getBlockValue(x, y + 1, z - 1);
					auto blockLeftUp = this->getBlockValue(x - 1, y + 1, z);
					auto blockRightUp = this->getBlockValue(x + 1, y + 1, z);

					auto blockAfterLeftUp = this->getBlockValue(x - 1, y + 1, z - 1);
					auto blockAfterRightUp = this->getBlockValue(x + 1, y + 1, z - 1);
					auto blockBeforeLeftUp = this->getBlockValue(x - 1, y + 1, z + 1);
					auto blockBeforeRightUp = this->getBlockValue(x + 1, y + 1, z + 1);

					auto blockBeforeDown = this->getBlockValue(x, y - 1, z + 1);
					auto blockAfterDown = this->getBlockValue(x, y - 1, z - 1);
					auto blockBeforeLeft = this->getBlockValue(x - 1, y, z + 1);
					auto blockAfterLeft = this->getBlockValue(x - 1, y, z - 1);
					auto blockBeforeRight = this->getBlockValue(x + 1, y, z + 1);
					auto blockAfterRight = this->getBlockValue(x + 1, y, z - 1);

					auto blockRightDown = this->getBlockValue(x + 1, y - 1, z);
					auto blockLeftDown = this->getBlockValue(x - 1, y - 1, z);


					auto blockBefore = this->getBlockValue(x, y, z + 1);
					if (blockBefore == BLOCK::Air || blockBefore == BLOCK::WATER) {
						glm::vec4 light = { 1, 1, 1, 1 };
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
						createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ + offset, size, coordinates.side, light);
					}
					auto blockAfter = this->getBlockValue(x, y, z - 1);
					if (blockAfter == BLOCK::Air || blockAfter == BLOCK::WATER) {
						glm::vec4 light = { 1, 1, 1, 1 };
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
						createPlane(PlaneType::VERTICALX, vertices, indices, positionX, positionY, positionZ - offset, size, coordinates.side, light);
					}

					auto blockRight = this->getBlockValue(x + 1, y, z);
					if (blockRight == BLOCK::Air || blockRight == BLOCK::WATER) {
						glm::vec4 light = { 1, 1, 1, 1 };
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
						createPlane(PlaneType::VERTICALZ, vertices, indices, positionX + offset, positionY, positionZ, size, coordinates.side, light);
					}
					auto blockLeft = this->getBlockValue(x - 1, y, z);
					if (blockLeft == BLOCK::Air || blockLeft == BLOCK::WATER) {
						glm::vec4 light = { 1, 1, 1, 1 };
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
						createPlane(PlaneType::VERTICALZ, vertices, indices, positionX - offset, positionY, positionZ, size, coordinates.side, light);
					}
					
					auto blockUp = this->getBlockValue(x, y + 1, z);
					if (blockUp == BLOCK::Air || blockUp == BLOCK::WATER) {
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

						
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + 0.5, positionZ, size, coordinates.top, light); // front-left front-right back.right back-left
					}
					auto blockDown = this->getBlockValue(x, y - 1, z);
					if (blockDown == BLOCK::Air || blockDown == BLOCK::WATER) {
						createPlane(PlaneType::HORIZONTAL, vertices, indices, positionX, positionY + 0.5, positionZ, size, coordinates.bottom, { 0.6, 0.6, 0.6, 0.6 });
					}
				}
			}
		}
	}

	//std::cout << "generateMesh" << std::endl;
	this->mesh = new glp::Mesh(glp::Mesh::DefaultVertexLayout, vertices, indices);

	this->status = ChunkStatus::MESH_GENERATED;
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
	case Leaf:
		block;
		block.top = this->textureAtlas.getTextureCoordinates(2, 1);
		block.side = this->textureAtlas.getTextureCoordinates(2, 1);
		block.bottom = this->textureAtlas.getTextureCoordinates(2, 1);
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
