#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace glp {

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
		void bind();

		void setUniform4f(std::string name, float v1, float v2, float v3, float v4);
		void setUniform1i(std::string name, int v1);
		void setUniformMat4f(std::string name, const glm::mat4& matrix);
	};
}