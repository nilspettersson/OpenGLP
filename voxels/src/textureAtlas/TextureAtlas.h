#pragma once
#include "OpenGLP.h"

struct TextureCoordinates {
	float x1;
	float x2;
	float y1;
	float y2;
};

class TextureAtlas {
public:
	TextureAtlas(std::string filename, glp::Texture::FILTER, int TextureSize);
	
	glp::Texture texture;
	int TextureSize;

	TextureCoordinates getTextureCoordinates(int x, int y);
};