#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader {
	struct ShaderProgramSouce {
		std::string vertexSource;
		std::string fragmentSource;
	};
	ShaderProgramSouce parseShader(const std::string& filepath);
	unsigned int compileShader(std::string& source, unsigned int type);
	void createShader(ShaderProgramSouce);
	unsigned int program;

public:
	Shader(std::string vertexShader, std::string fragmentShader);
	Shader(std::string filepath);
};