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
	static struct ShaderProgramSouce {
		std::string vertexSource;
		std::string fragmentSource;
	};
	static ShaderProgramSouce parseShader(const std::string& filepath);
	Shader(std::string vertexShader, std::string fragmentShader);
};