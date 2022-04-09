#pragma once

namespace glp {
	class Input {
		void* window;
	public:
		Input() { };
		Input(void*);
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