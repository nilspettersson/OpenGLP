#pragma once
#include "OpenGLP.h"
#include "./ChunkGenerator.h"
#include "../textureAtlas/TextureAtlas.h"

class ChunkManager {
private:
    std::vector<ChunkGenerator> chunks;
    TextureAtlas textureAtlas;
    glp::Shader shader;

public:
    ChunkManager(int chunks, int chunkWidth, int chunkHeight);

    std::vector<glp::Entity> entities;
    std::vector<glp::Vao> models;

    void CreateEntities();

};