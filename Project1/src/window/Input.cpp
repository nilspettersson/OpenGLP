#include "Input.h"

using namespace glp;

bool Input::isKeyDown(GLFWwindow* window, int keycode) {
	int state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}