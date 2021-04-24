#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader {
	unsigned int shaderId;
	struct ShaderProgramSouce {
		std::string vertexSource;
		std::string fragmentSource;
	};
	ShaderProgramSouce parseShader(const std::string& filepath);
	unsigned int compileShader(std::string& source, unsigned int type);
	void createShader(ShaderProgramSouce);
	int getUniformLocation(std::string name);

public:
	Shader(std::string vertexShader, std::string fragmentShader);
	Shader(std::string filepath);

	unsigned int getShaderId();

	void setUniform4f(std::string name, float v1, float v2, float v3, float v4);
};