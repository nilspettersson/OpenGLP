#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>


class Vao {
	unsigned int vaoArrayId;
	std::vector<int> vertexLayout;
	int vertexSize;
	unsigned int vaoElementId;
	void setVertexSize();

public:
	Vao(std::vector<int> vertexLayout);
};
