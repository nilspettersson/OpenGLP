#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace glp {
	class Vao {
		unsigned int vaoArrayId;
		std::vector<int> vertexLayout;
		int vertexSize;
		int indicesCount;
		int verticesCount;
		unsigned int vaoElementId;
		void setVertexSize();

	public:
		Vao(std::vector<int> vertexLayout, const unsigned int* indices, int indicesCount, const float* vertices, int verticesCount);
		void render();
		unsigned int getVaoArrayId();
		unsigned int getVaoElementId();
		int getVertexSize();
	};

}