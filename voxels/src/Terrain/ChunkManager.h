#pragma once
#include "OpenGLP.h"
#include "./ChunkGenerator.h"
#include "../textureAtlas/TextureAtlas.h"

class ChunkManager {
private:
    //std::vector<ChunkGenerator> chunks;
    std::unordered_map<std::string, ChunkGenerator> chunks;
    TextureAtlas textureAtlas;
    glp::Shader shader;
    int chunkWidth;
    int chunkHeight;
    int chunkCount;

public:
    ChunkManager(int chunks, int chunkWidth, int chunkHeight);

    std::vector<glp::Entity*> entities;

    void updateChunks(int originX, int originY);

    void CreateEntities();

};