#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace glp {
	class Input {
		GLFWwindow* window;
	public:
		Input() { };
		Input(GLFWwindow*);
		bool isKeyDown(int);
		bool isMouseButtonDown(int);
		bool getMouseX(int);
		bool getMouseY(int);
	};
}