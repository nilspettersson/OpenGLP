#include "Camera.h"
#include <iostream>


glp::Camera::Camera(int width, int height) {
	this->width = (float)width;
	this->height = (float)height;
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->fov = glm::radians(70.0);
	this->projection = glm::mat4();
}

glm::mat4 glp::Camera::getProjection() {
	glm::mat4 output = glm::translate(this->projection, glm::vec3(this->x, this->y, this->z));
	return output;
}

glm::mat4 glp::Camera::getViewMatrix() {
	return glm::mat4();
}

float glp::Camera::getWidth() {
	return this->width;
}

float glp::Camera::getHeight() {
	return this->height;
}

void glp::Camera::setWidth(float width) {
	this->width = width;
}

void glp::Camera::setHeight(float height) {
	this->height = height;
}

float glp::Camera::getX() {
	return this->x;
}

float glp::Camera::getY() {
	return this->y;
}

void glp::Camera::setX(float x) {
	this->x = x;
}

void glp::Camera::setY(float y) {
	this->y = y;
}

glp::Camera2d::Camera2d(Window &window) : Camera(window.getWidth(), window.getHeight()) {
	this->projection = glm::ortho(this->width / 2, this->width / 2, this->height / 2, -this->height / 2, -1.0f, 1.0f);
}

glp::Camera2d::Camera2d(int width, int height) : Camera(width, height) {
	this->projection = glm::ortho(this->width / 2, this->width / 2, this->height / 2, -this->height / 2, -1.0f, 1.0f);
}


glp::Camera3d::Camera3d(Window& window) : Camera(window.getWidth(), window.getHeight()) {
	this->rotation = glm::vec3();
	this->projection = glm::perspective(this->fov, this->width / this->height, 0.1f, 10000.0f);
}

glp::Camera3d::Camera3d(int width, int height) : Camera(width, height) {
	this->rotation = glm::vec3();
	this->projection = glm::perspective(this->fov, this->width / this->height, 0.1f, 10000.0f);
}

void glp::Camera3d::updateProjection() {
	this->projection = glm::perspective(this->fov, this->width / this->height, 0.1f, 10000.0f);
}

glm::mat4 glp::Camera3d::getProjection() {
	glm::mat4 output = this->projection;

	/*glm::qua<float> quaternion = glm::qua<float>();
	quaternion = glm::angleAxis(this->rotation.x, glm::vec3(1, 0, 0));
	quaternion = glm::rotate(quaternion, this->rotation.y, glm::vec3(0, 1, 0));

	output = output * glm::mat4_cast(quaternion);

	output = glm::translate(output, glm::vec3(this->x, this->y, this->z));*/
	return output;
}

glm::mat4 glp::Camera3d::getViewMatrix() {
	glm::qua<float> quaternion = glm::qua<float>();
	quaternion = glm::angleAxis(this->rotation.x, glm::vec3(1, 0, 0));
	quaternion = glm::rotate(quaternion, this->rotation.y, glm::vec3(0, 1, 0));

	glm::mat4 output = glm::mat4_cast(quaternion);

	output = glm::translate(output, glm::vec3(this->x, this->y, this->z));

	return output;
}




float glp::Camera3d::getFov() {
	return this->fov;
}

void glp::Camera3d::setFov(float fovInDegrees) {
	this->fov = glm::radians(fovInDegrees);
	this->updateProjection();
}

float glp::Camera3d::getZ() {
	return this->z;
}

void glp::Camera3d::setZ(float z) {
	this->z = z;
}

void glp::Camera3d::rotateX(float angle) {
	this->rotation.x += angle;
}

void glp::Camera3d::rotateY(float angle) {
	this->rotation.y += angle;
}

void glp::Camera3d::moveForward(float amount) {
	glm::vec3 pos = glm::rotate(glm::quat(this->rotation), glm::vec3(0, 0, amount));
	this->x -= pos.x;
	this->y -= pos.y;
	this->z += pos.z;
}

void glp::Camera3d::moveBackward(float amount) {
	glm::vec3 pos = glm::rotate(glm::quat(this->rotation), glm::vec3(0, 0, -amount));
	this->x -= pos.x;
	this->y -= pos.y;
	this->z += pos.z;
}

void glp::Camera3d::moveLeft(float amount) {
	glm::vec3 pos = glm::rotate(glm::quat(this->rotation), glm::vec3(-amount, 0, 0));
	this->x -= pos.x;
	this->y -= pos.y;
	this->z += pos.z;
}

void glp::Camera3d::moveRight(float amount) {
	glm::vec3 pos = glm::rotate(glm::quat(this->rotation), glm::vec3(amount, 0, 0));
	this->x -= pos.x;
	this->y -= pos.y;
	this->z += pos.z;
}

void glp::Camera3d::moveUp(float amount) {
	glm::vec3 pos = glm::rotate(glm::quat(this->rotation), glm::vec3(0, amount, 0));
	this->x -= pos.x;
	this->y -= pos.y;
	this->z += pos.z;
}

void glp::Camera3d::moveDown(float amount) {
	glm::vec3 pos = glm::rotate(glm::quat(this->rotation), glm::vec3(0, -amount, 0));
	this->x -= pos.x;
	this->y -= pos.y;
	this->z += pos.z;
}
