#pragma once
#include "OpenGLP.h"
#include "FastNoiseLite.h"
#include "../textureAtlas/TextureAtlas.h"

enum ChunkStatus { NONE, TERAIN_GENERATED, MESH_GENERATED, RENDERED };

enum BLOCK {
    Air, GRASS, DIRT, SAND, WATER, WOOD, Leaf
};

struct Block {
    TextureCoordinates top;
    TextureCoordinates side;
    TextureCoordinates bottom;
};

class ChunkGenerator {
private:
    std::vector<std::vector<std::vector<int>>> cells;
    TextureAtlas& textureAtlas;

    int GetTerainHeight(float x, float z, FastNoiseLite noise);
public:
    ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, float detailMultiplier, TextureAtlas& textureAtlas, std::unordered_map<std::string, ChunkGenerator*>& chunksList);
    ~ChunkGenerator();

    float detailMultiplier;
    int chunkX;
    int chunkZ;
    int maxHeight;
    int chunkWidth;
    glp::Mesh* mesh;
    ChunkStatus status;

    glp::Entity *chunkEntity;

    std::unordered_map<std::string, ChunkGenerator*>& chunksList;

    void generateTerain();

    void generateMesh();

    int getBlockValue(int x, int y, int z);

    Block GetBlock(BLOCK block);

};
