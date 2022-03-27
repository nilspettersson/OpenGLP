#include "Material.h"

using namespace glp;

glp::Material::Material(Shader *shader) {
	this->shader = shader;

	this->floatUniforms = std::unordered_map<std::string, float>();
	this->intUniforms = std::unordered_map<std::string, int>();
	this->vec2fUniforms = std::unordered_map<std::string, glm::vec2>();
	this->vec3fUniforms = std::unordered_map<std::string, glm::vec3>();
}
void Material::setProperty(std::string name, float value) {
	this->floatUniforms[name] = value;
}

void glp::Material::setProperty(std::string name, int value) {
	this->intUniforms[name] = value;
}

void glp::Material::setProperty(std::string name, float v1, float v2) {
	glm::vec2 property = { v1, v2 };
	this->vec2fUniforms[name] = property;
}

void glp::Material::setProperty(std::string name, float v1, float v2, float v3) {
	glm::vec3 property = { v1, v2, v3 };
	this->vec3fUniforms[name] = property;
}

void glp::Material::setProperty(std::string name, float v1, float v2, float v3, float v4) {
	glm::vec4 property = { v1, v2, v3, v4 };
	this->vec3fUniforms[name] = property;
}

Shader* glp::Material::getShader() {
	return this->shader;
}

void glp::Material::setShader(Shader* shader) {
	this->shader = shader;
}

void glp::Material::updateUniforms() {
	for(auto& it : this->floatUniforms) {
		shader->setUniform1f(it.first, it.second);
	}
	for (auto& it : this->intUniforms) {
		shader->setUniform1i(it.first, it.second);
	}
	for (auto& it : this->vec2fUniforms) {
		shader->setUniform2f(it.first, it.second.x, it.second.y);
	}
	for (auto& it : this->vec3fUniforms) {
		shader->setUniform3f(it.first, it.second.x, it.second.y, it.second.z);
	}
	for (auto& it : this->vec4fUniforms) {
		shader->setUniform4f(it.first, it.second.x, it.second.y, it.second.z, it.second.w);
	}
}
