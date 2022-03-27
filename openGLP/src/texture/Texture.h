#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

namespace glp {
	class Texture {
		unsigned int textureId;
		std::string filePath;
		unsigned char* buffer;
		int width, height, bpp;
	public:
		enum FILTER {
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR
		};

		Texture(const std::string& path, FILTER filter);
		~Texture();

		void bind(int slot = 0);
		void unbind();

		int getWidth();
		int getHeight();

	};
}