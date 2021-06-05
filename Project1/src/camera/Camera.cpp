#include "Camera.h"
#include <iostream>

glp::Camera::Camera(int width, int height) {
	this->width = (float)width;
	this->height = (float)height;
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

glm::mat4 glp::Camera::getProjection() {
	glm::mat4 output = glm::translate(this->projection, glm::vec3(this->x, this->y, this->z));
	return output;
}

int glp::Camera::getWidth() {
	return this->width;
}

int glp::Camera::getHeight() {
	return this->height;
}

int glp::Camera::getX() {
	return this->x;
}

int glp::Camera::getY() {
	return this->y;
}

void glp::Camera::setX(float x) {
	this->x = x;
}

void glp::Camera::setY(float y) {
	this->y = y;
}


glp::Camera2d::Camera2d(int width, int height) : Camera(width, height) {
	this->projection = glm::ortho(-this->width / 2, this->width / 2, this->height / 2, -this->height / 2, -1.0f, 1.0f);
}




glp::Camera3d::Camera3d(int width, int height) : Camera(width, height) {
	this->width = (float)width;
	this->height = (float)height;
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->projection = glm::perspective(2.2f, this->width / this->height, 0.1f, 1000.0f);
}

int glp::Camera3d::getZ() {
	return this->z;
}

void glp::Camera3d::setZ(float z) {
	this->z = z;
}