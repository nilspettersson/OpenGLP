#pragma once
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../shader/Shader.h"

namespace glp {
	class Material {
		Shader *shader;
		std::unordered_map<std::string, float> floatUniforms;
		std::unordered_map<std::string, int> intUniforms;
		std::unordered_map<std::string, glm::vec2> vec2fUniforms;
		std::unordered_map<std::string, glm::vec3> vec3fUniforms;

	public:
		Material(Shader *shader);

		void addProperty(std::string name, float value);
		void addProperty(std::string name, int value);
		void addProperty(std::string name, float v1, float v2);
		void addProperty(std::string name, float v1, float v2, float v3);

		Shader *getShader();
		void setShader(Shader* shader);
	};
}