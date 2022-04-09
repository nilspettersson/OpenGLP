#include "Input.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glp;

Input::Input(void* window) {
	this->window = window;
}

bool Input::isKeyDown(int keycode) {
	int state = glfwGetKey((GLFWwindow*)this->window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonDown(int button) {
	int state = glfwGetMouseButton((GLFWwindow*)this->window, button);
	return state == GLFW_PRESS;
}

double Input::getMouseX() {
	double x, y;
	glfwGetCursorPos((GLFWwindow*)this->window, &x, &y);
	return x;
}

double Input::getMouseY() {
	double x, y;
	glfwGetCursorPos((GLFWwindow*)this->window, &x, &y);
	return y;
}

void glp::Input::setCursorHidden() {
	glfwSetInputMode((GLFWwindow*)this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void glp::Input::setCursorDisabled(bool useRawMouseData) {
	glfwSetInputMode((GLFWwindow*)this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (useRawMouseData || glfwRawMouseMotionSupported()) {
		glfwSetInputMode((GLFWwindow*)this->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}

void glp::Input::setCursorNormal() {
	glfwSetInputMode((GLFWwindow*)this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void glp::Input::setCursorPosition(float x, float y) {
	glfwSetCursorPos((GLFWwindow*)this->window, x, y);
}
