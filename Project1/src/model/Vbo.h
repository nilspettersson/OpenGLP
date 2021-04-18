#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static enum VboType {
	FLOAT = GL_FLOAT,
	INT = GL_INT,
	SHORT = GL_SHORT
};

class Vbo {
public:
	int type;
	int size;
	Vbo(unsigned int size, VboType);
};