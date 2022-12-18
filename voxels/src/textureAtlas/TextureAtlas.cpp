#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(std::string filename, glp::Texture::FILTER, int TextureSize): texture(filename, glp::Texture::FILTER::NEAREST) {
	this->TextureSize = TextureSize;
}

TextureCoordinates TextureAtlas::getTextureCoordinates(int x, int y) {
	float textureWidth = (float)this->TextureSize / (float)this->texture.getWidth();
	float textureHeight = (float)this->TextureSize / (float)this->texture.getWidth();

	y = (float)this->texture.getWidth() / (float)this->TextureSize - y - 1;

	TextureCoordinates textureCoordinates;
	textureCoordinates.x1 = textureWidth * x;
	textureCoordinates.x2 = textureWidth * x + textureWidth;
	textureCoordinates.y1 = textureWidth * y;
	textureCoordinates.y2 = textureWidth * y + textureHeight;

	return textureCoordinates;
}


