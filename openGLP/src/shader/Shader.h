#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "glm/gtc/matrix_transform.hpp"

namespace glp {

	class Shader {
		unsigned int shaderId;
		std::unordered_map<std::string, int> uniformLocations;
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
		void setUniform3f(std::string name, float v1, float v2, float v3);
		void setUniform2f(std::string name, float v1, float v2);
		void setUniform1f(std::string name, float value);
		void setUniform1i(std::string name, int value);
		void setUniform1iv(std::string name, const int* values);
		void setUniformMat4f(std::string name, const glm::mat4& matrix);
	};
}