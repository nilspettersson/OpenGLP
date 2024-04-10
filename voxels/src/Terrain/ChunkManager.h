#pragma once
#include "OpenGLP.h"
#include "./ChunkGenerator.h"
#include "../textureAtlas/TextureAtlas.h"
#include <mutex>

class ChunkManager {
private:
    TextureAtlas textureAtlas;
    glp::Shader shader;
    int chunkWidth;
    int chunkHeight;
    int chunkCount;

    std::thread chunkThread;
    std::thread meshThread;
    bool isRunning;

public:
    ChunkManager(int chunks, int chunkWidth, int chunkHeight);
    ~ChunkManager() {
        isRunning = false;
        if (meshThread.joinable()) {
            meshThread.join();
        }
    }
    int originX;
    int originZ;
    std::unordered_map<std::string, ChunkGenerator*> chunks;
    std::vector<glp::Entity*> entities;
    
    std::mutex chunksMutex;

    void updateChunks(int originX, int originY);

    void generateChunks();
    

    void CreateEntities();

    void CreateChunkMesh();

};