#include "Entity.h"

using namespace glp;

glp::Entity::Entity(Vao* model, Shader* shader, float scale): material(shader), model(model) {
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->scale = scale;
	this->textures = std::vector<Texture*>();
}

Shader& Entity::getShader() { 
	return *this->material.getShader();
}

void Entity::setShader(Shader& shader) {
	this->material.setShader(&shader);
}

Vao& Entity::getModel() {
	return *this->model;
}

void Entity::setModel(Vao* model) {
	this->model = model;
}

std::vector<Texture*> Entity::getTextures() {
	return this->textures;
}

void Entity::addTexture(Texture* texture) {
	this->textures.push_back(texture);
}

void glp::Entity::setX(float x) {
	this->x = x;
}

float glp::Entity::getX() {
	return this->x;
}

void glp::Entity::setY(float y) {
	this->y = y;
}

float glp::Entity::getY() {
	return this->y;
}

void glp::Entity::setZ(float z) {
	this->z = z;
}

float glp::Entity::getZ() {
	return this->z;
}

void glp::Entity::setScale(float scale) {
	this->scale = scale;
}

float glp::Entity::getScale() {
	return this->scale;
}

Material glp::Entity::getMaterial() {
	return this->material;
}

void glp::Entity::setMaterial(Material material) {
	this->material = material;
}

std::vector<Entity> glp::Entity::getChildren() {
	return this->children;
}

void glp::Entity::addChild(Entity entity) {
	this->children.push_back(entity);
}
