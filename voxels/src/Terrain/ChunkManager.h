#pragma once
#include "OpenGLP.h"
#include "./ChunkGenerator.h"
#include "../textureAtlas/TextureAtlas.h"
#include <mutex>
#include <shared_mutex>

class ChunkManager {
private:
    TextureAtlas textureAtlas;
    glp::Shader shader;
    int chunkWidth;
    int chunkHeight;
    int chunkCount;

    std::vector<std::thread> meshThreads;
    bool isRunning;

public:
    ChunkManager(int chunks, int chunkWidth, int chunkHeight);
    ~ChunkManager();
    int originX;
    int originZ;
    std::unordered_map<int64_t, ChunkGenerator*> chunks;
    
    mutable std::shared_mutex chunksMutex;

    void updateChunks(int originX, int originY);

    void generateChunks();
    

    void CreateEntities();

    void CreateChunkMesh();

    static int64_t getKey(int32_t x, int32_t z) {
        return (static_cast<std::uint64_t>(x) << 32) | (static_cast<std::uint64_t>(z) & 0xFFFFFFFF);
    }

};