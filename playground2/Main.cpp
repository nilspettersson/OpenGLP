#include "OpenGLP.h"

int main(void) {
	glp::Window window = glp::Window();
	glp::Camera3d camera = glp::Camera3d(window);

    glp::Vao vao(glp::Mesh::Primitives::Plane(1, 1), false);
    glp::Shader shader("res/shaders/main.shader");
    glp::Entity plane = glp::Entity(vao, &shader, 100);
    plane.setZ(-100);

    glp::Renderer renderer = glp::Renderer(camera);

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);
	while (!window.shouldClose()) {
		window.drawInit();

        renderer.render(plane);

        float speed = 0.1;
        if (window.getInput().isKeyDown(GLFW_KEY_W)) {
            camera.moveForward(speed);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_S)) {
            camera.moveBackward(speed);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_A)) {
            camera.moveLeft(speed);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_D)) {
            camera.moveRight(speed);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_E)) {
            camera.moveUp(speed);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_Q)) {
            camera.moveDown(speed);
        }
        camera.rotateX(window.getInput().getMouseY() / 800);
        camera.rotateY(window.getInput().getMouseX() / 800);
        window.getInput().setCursorPosition(0, 0);
		
		window.clean();
	}

	return 0;
}