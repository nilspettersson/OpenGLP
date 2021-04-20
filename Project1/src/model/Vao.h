#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>


class Vao {
	bool dynamicDraw;
	unsigned int buffer;
	std::vector<int> vertexLayout;
	int vertexSize;

public:
	Vao(std::vector<int> vertexLayout, bool dynamicDraw);
};
