#include "Input.h"

using namespace glp;

Input::Input(GLFWwindow* window) {
	this->window = window;
}

bool Input::isKeyDown(int keycode) {
	int state = glfwGetKey(this->window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonDown(int button) {
	int state = glfwGetMouseButton(this->window, button);
	return state == GLFW_PRESS;
}

double Input::getMouseX() {
	double x, y;
	glfwGetCursorPos(this->window, &x, &y);
	return x;
}

double Input::getMouseY() {
	double x, y;
	glfwGetCursorPos(this->window, &x, &y);
	return y;
}

void glp::Input::setCursorHidden() {
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void glp::Input::setCursorDisabled(bool useRawMouseData) {
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (useRawMouseData || glfwRawMouseMotionSupported()) {
		glfwSetInputMode(this->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}

void glp::Input::setCursorNormal() {
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void glp::Input::setCursorPosition(float x, float y) {
	glfwSetCursorPos(this->window, x, y);
}
