#pragma once
#include "OpenGLP.h"
#include "FastNoiseLite.h"
#include "../textureAtlas/TextureAtlas.h"
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <atomic>

enum ChunkStatus { NONE, TERAIN_GENERATED, DECORATIONS_GENERATED, MESH_GENERATED, RENDERED };

enum BLOCK {
    Air, GRASS, DIRT, SAND, WATER, WOOD, LEAF, SNOW
};

struct Block {
    TextureCoordinates top;
    TextureCoordinates side;
    TextureCoordinates bottom;
};

class ChunkGenerator {
private:
    std::vector<int16_t> cells;
    std::vector<glm::vec4> decorations;
    std::vector<glm::vec4> overflowLeft;
    std::vector<glm::vec4> overflowRight;
    std::vector<glm::vec4> overflowBefore;
    std::vector<glm::vec4> overflowAfter;
    TextureAtlas& textureAtlas;

    enum BIOMES {
        FLAT,
        FOREST,
        DESERT
    };

    struct BiomeInformation {
        BIOMES biome;
        float temperature;
        float humidity;
    };

    int GetTerainHeight(float x, float z, FastNoiseLite noise);
    BiomeInformation GetBiome(float x, float z, FastNoiseLite noise);

    
public:
    ChunkGenerator(int chunkX, int chunkZ, int chunkWidth, int maxHeight, float detailMultiplier, TextureAtlas& textureAtlas, std::unordered_map<int64_t, ChunkGenerator*>& chunksList, std::shared_mutex& chunkListLock);
    ~ChunkGenerator();

    std::shared_mutex chunkLock;
    std::shared_mutex& chunkListLock;

    float detailMultiplier;
    int chunkX;
    int chunkZ;
    int maxHeight;
    int chunkWidth;
    glp::Mesh* mesh;
    glp::Mesh* meshTransparent;
    std::atomic<ChunkStatus> status;

    glp::Entity *chunkEntity;
    glp::Entity* chunkEntityTransparent;

    std::unordered_map<int64_t, ChunkGenerator*>& chunksList;

    void generateTerain();


    void generateDecorations();
    void addCell(int x, int z, int y, BLOCK block);

    void generateMesh();

    int getBlockValue(int x, int y, int z, std::unordered_map<int64_t, ChunkGenerator*> &closeChunks);

    Block GetBlock(BLOCK block);

    bool isTransparent(BLOCK block);

    int getIndex(int x, int z, int y) const {
        int realChunkWidth = chunkWidth * detailMultiplier;
        return x * realChunkWidth * maxHeight + z * maxHeight + y;
    }

    glm::ivec3 getXYZ(int index) {
        int realChunkWidth = chunkWidth * detailMultiplier;
        int y = index % maxHeight;
        int z = (index / maxHeight) % realChunkWidth;
        int x = index / (realChunkWidth * maxHeight);
        return { x, y, z };
    }


};
