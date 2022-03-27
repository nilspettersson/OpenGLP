#pragma once
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../shader/Shader.h"
#include <iostream>

namespace glp {
	class Material {
		Shader *shader;
		std::unordered_map<std::string, float> floatUniforms;
		std::unordered_map<std::string, int> intUniforms;
		std::unordered_map<std::string, glm::vec2> vec2fUniforms;
		std::unordered_map<std::string, glm::vec3> vec3fUniforms;
		std::unordered_map<std::string, glm::vec4> vec4fUniforms;

	public:
		Material(Shader *shader);

		void setProperty(std::string name, float value);
		void setProperty(std::string name, int value);
		void setProperty(std::string name, float v1, float v2);
		void setProperty(std::string name, float v1, float v2, float v3);
		void setProperty(std::string name, float v1, float v2, float v3, float v4);

		Shader *getShader();
		void setShader(Shader* shader);

		void updateUniforms();
	};
}