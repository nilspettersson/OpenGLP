#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace glp {
	enum class DataType {
		INT = GL_INT,
		SHORT = GL_SHORT,
		BYTE = GL_BYTE,
		FLOAT = GL_FLOAT,
	};

	struct VertexLayout {
		unsigned int attribCount;
		DataType dataType;

		VertexLayout(unsigned int attribCount, DataType dataType);
	};

	class Vao {
		unsigned int vaoArrayId;
		std::vector<VertexLayout> vertexLayout;
		int vertexSize;
		int indicesCount;
		int verticesCount;
		unsigned int vaoElementId;
		void setVertexSize();

	public:
		Vao(std::vector<VertexLayout> vertexLayout, const unsigned int* indices, int indicesCount, const float* vertices, int verticesCount);
		void render();
		void bind();
		unsigned int getVaoArrayId();
		unsigned int getVaoElementId();
		int getVertexSize();
	};
}