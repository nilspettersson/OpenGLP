#pragma once
#include "Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace glp {
	static class Input {
	public:
		static bool isKeyDown(GLFWwindow*, int);
	};
}