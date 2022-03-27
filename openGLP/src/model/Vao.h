#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Mesh.h"

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
		int vertexSize;
		int indicesCount;
		int verticesCount;
		unsigned int vaoElementId;
		void setVertexSize();
		bool dynamic;

	public:
		Vao(std::vector<int> vertexLayout, const unsigned int* indices, int indicesCount, const float* vertices, int verticesCount, bool dynamic);
		Vao(Mesh mesh, bool dynamic);
		void render();
		void bind();
		unsigned int getVaoArrayId();
		unsigned int getVaoElementId();
		int getVertexSize();

		void updateVertices(int offset, int size, const float* data);
		void setVertices(int size, const float* data);

		void updateIndices(int offset, int size, const float* data);
		void setIndices(int size, const float* data);
	};
}