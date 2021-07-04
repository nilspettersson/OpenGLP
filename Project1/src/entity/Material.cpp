#include "Material.h"

using namespace glp;
glp::Material::Material() {
	this->floatUniforms = std::unordered_map<std::string, float>();
	this->intUniforms = std::unordered_map<std::string, int>();
	this->vec2fUniforms = std::unordered_map<std::string, glm::vec2>();
	this->vec3fUniforms = std::unordered_map<std::string, glm::vec3>();
}
void Material::addProperty(std::string name, float value) {
	this->floatUniforms[name] = value;
}

void glp::Material::addProperty(std::string name, int value) {
	this->intUniforms[name] = value;
}

void glp::Material::addProperty(std::string name, float v1, float v2) {
	glm::vec2 property = { v1, v2 };
	this->vec2fUniforms[name] = property;
}

void glp::Material::addProperty(std::string name, float v1, float v2, float v3) {
	glm::vec3 property = { v1, v2, v3 };
	this->vec3fUniforms[name] = property;
}
