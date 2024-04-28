#pragma once
#include "OpenGLP.h"
#include "./ChunkGenerator.h"
#include "../textureAtlas/TextureAtlas.h"
#include <mutex>
#include <shared_mutex>
#include <functional>

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

    void spiral(std::function<int(int x, int y)> processChunk);
    int addChunk(int x, int y);
    int generateChunk(int x, int y);
    int generateDecorations(int x, int y);
    int CreateMesh(int x, int y);

    void updateChunks(int originX, int originY);
    void CreateEntities();

    void generateChunks();
    void CreateChunkMesh();

    static int64_t getKey(int32_t x, int32_t z) {
        return (static_cast<std::uint64_t>(x) << 32) | (static_cast<std::uint64_t>(z) & 0xFFFFFFFF);
    }

};