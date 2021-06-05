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

bool Input::getMouseX(int button) {
	double x, y;
	glfwGetCursorPos(this->window, &x, &y);
	return x;
}

bool Input::getMouseY(int button) {
	double x, y;
	glfwGetCursorPos(this->window, &x, &y);
	return y;
}