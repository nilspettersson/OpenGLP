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
		double getMouseX();
		double getMouseY();

		void setCursorHidden();
		void setCursorDisabled(bool useRawMouseData);
		void setCursorNormal();
		void setCursorPosition(float x, float y);
	};
}