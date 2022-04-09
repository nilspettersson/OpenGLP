#pragma once
#include <string>

namespace glp {
	class Texture {
		unsigned int textureId;
		std::string filePath;
		unsigned char* buffer;
		int width, height, bpp;
	public:
		enum class FILTER {
			NEAREST,
			LINEAR
		};

		Texture(const std::string& path, FILTER filter);
		~Texture();

		void bind(int slot = 0);
		void unbind();

		int getWidth();
		int getHeight();

	};
}