#include "OpenGLP.h"

int main(void) {
	glp::Window window = glp::Window();
	glp::Camera3d camera = glp::Camera3d(window);

	//glp::Entity plane = glp::Entity()

	while (!window.shouldClose()) {
		window.drawInit();
		
		window.clean();
	}

	return 0;
}