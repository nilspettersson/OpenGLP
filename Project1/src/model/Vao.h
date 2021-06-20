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

	class Vao {
		unsigned int vaoArrayId;
		std::vector<int> vertexLayout;
		std::vector<float>* vertexBuffer;
		int vertexSize;
		int indicesCount;
		int verticesCount;
		unsigned int vaoElementId;
		void setVertexSize();
		bool dynamic;

	public:
		Vao(std::vector<int> vertexLayout, const unsigned int* indices, int indicesCount, const float* vertices, int verticesCount, bool dynamic);
		void render();
		void bind();
		unsigned int getVaoArrayId();
		unsigned int getVaoElementId();
		int getVertexSize();
	};
}