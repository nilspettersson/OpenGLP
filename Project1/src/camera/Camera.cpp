#include "Camera.h"


glp::Camera2d::Camera2d(int width, int height) {
	this->width = (float) width;
	this->height = (float) height;
	this->x = 0;
	this->y = 0;

	this->projection = glm::ortho(-this->width / 2, this->width / 2, this->height / 2, -this->height / 2, -1.0f, 1.0f);
}

glm::mat4 glp::Camera2d::getProjection() {
	glm::mat4 output = glm::translate(this->projection, glm::vec3(this->x, this->y, 0));
	return output;
}

int glp::Camera2d::getWidth() {
	return this->width;
}

int glp::Camera2d::getHeight() {
	return this->height;
}

int glp::Camera2d::getX() {
	return this->x;
}

int glp::Camera2d::getY() {
	return this->y;
}

void glp::Camera2d::setX(float x) {
	this->x = x;
}

void glp::Camera2d::setY(float y) {
	this->y = y;
}




glp::Camera3d::Camera3d(int width, int height) {
	this->width = (float)width;
	this->height = (float)height;
	this->x = 0;
	this->y = 0;
	this->z = 0;

	this->projection = glm::perspective(80.0f, this->width / this->height, 0.1f, 1000.0f);
}

glm::mat4 glp::Camera3d::getProjection() {
	glm::mat4 output = glm::translate(this->projection, glm::vec3(this->x, this->y, this->z - 1000));
	return output;
}

int glp::Camera3d::getWidth() {
	return this->width;
}

int glp::Camera3d::getHeight() {
	return this->height;
}

int glp::Camera3d::getX() {
	return this->x;
}

int glp::Camera3d::getY() {
	return this->y;
}

int glp::Camera3d::getZ() {
	return this->z;
}

void glp::Camera3d::setX(float x) {
	this->x = x;
}

void glp::Camera3d::setY(float y) {
	this->y = y;
}

void glp::Camera3d::setZ(float z) {
	this->y = y;
}
