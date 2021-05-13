#include "Entity.h"

using namespace glp;

glp::Entity::Entity(Vao* model, Shader* shader) {
	this->model = model;
	this->shader = shader;
}

Shader* Entity::getShader() { 
	return this->shader;
}

void Entity::setShader(Shader* shader) {
	this->shader = shader;
}

Vao* Entity::getModel() {
	return this->model;
}

void Entity::setModel(Vao* model) {
	this->model = model;
}
