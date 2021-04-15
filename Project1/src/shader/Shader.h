#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader {
	unsigned int compileShader(std::string& source, unsigned int type);
	unsigned int program;

public:
	Shader(std::string vertexShader, std::string fragmentShader);
};