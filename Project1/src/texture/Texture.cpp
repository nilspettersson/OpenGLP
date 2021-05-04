#include "Texture.h"
#include "../vendor/stb/stb_image.h"

using namespace glp;

Texture::Texture(const std::string& path, FILTER filter = FILTER::NEAREST) {
	this->textureId = 0;
	this->filePath = path;
	this->buffer = nullptr;
	this->width = 0;
	this->height = 0;
	this->bpp = 0;

	stbi_set_flip_vertically_on_load(1);
	this->buffer = stbi_load(filePath.c_str(), &width, &height, &bpp, 4);

	glGenTextures(1, &this->textureId);
	glBindTexture(GL_TEXTURE_2D, this->textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (buffer) {
		stbi_image_free(buffer);
	}
}

Texture::~Texture() {
	glDeleteTextures(1, &textureId);
}

void Texture::bind(int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getWidth() {
	return 0;
}

int Texture::getHeight() {
	return 0;
}
