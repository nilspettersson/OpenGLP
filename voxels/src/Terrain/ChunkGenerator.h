#pragma once
#include "OpenGLP.h"
#include "FastNoiseLite.h"
#include "../textureAtlas/TextureAtlas.h"

enum ChunkStatus { NONE, TERAIN_GENERATED, MESH_GENERATED };

enum BLOCK {
    Air, GRASS, DIRT
};

struct Block {
    TextureCoordinates top;
    TextureCoordinates side;
    TextureCoordinates bottom;
};

class ChunkGenerator {
private:
    std::vector<std::vector<std::vector<int>>> cells;
    float detailMultiplier;
    TextureAtlas& textureAtlas;

    int GetTerainHeight(float x, float z, FastNoiseLite noise);
public:
    ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, float detailMultiplier, TextureAtlas& textureAtlas, std::unordered_map<std::string, ChunkGenerator>& chunksList);

    int chunkX;
    int chunkZ;
    int maxHeight;
    int chunkWidth;
    ChunkStatus status;

    std::unordered_map<std::string, ChunkGenerator>& chunksList;

    void generateTerain();

    glp::Mesh generateMesh();

    int getBlockValue(int x, int y, int z);

    Block GetBlock(BLOCK block);

};
