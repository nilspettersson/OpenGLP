#include "OpenGLP.h"

int main(void) {
	auto window = glp::Window();
    auto camera = glp::Camera3d(window);

    auto vao = glp::Vao(glp::Mesh::Primitives::Plane(1, 1), false);
    auto shader = glp::Shader("res/shaders/main.shader");
    auto plane = glp::Entity(vao, &shader, 100);
    plane.setZ(-100);

    auto renderer = glp::Renderer(camera);
    
    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);
	while (!window.shouldClose()) {
		window.drawStart();

        renderer.render(plane);

        float speed = 0.1;
        if (window.getInput().isKeyDown(GLP_KEY_W)) {
            camera.moveForward(speed);
        }
        if (window.getInput().isKeyDown(GLP_KEY_S)) {
            camera.moveBackward(speed);
        }
        if (window.getInput().isKeyDown(GLP_KEY_A)) {
            camera.moveLeft(speed);
        }
        if (window.getInput().isKeyDown(GLP_KEY_D)) {
            camera.moveRight(speed);
        }
        if (window.getInput().isKeyDown(GLP_KEY_E)) {
            camera.moveUp(speed);
        }
        if (window.getInput().isKeyDown(GLP_KEY_Q)) {
            camera.moveDown(speed);
        }
        camera.rotateX(window.getInput().getMouseY() / 800);
        camera.rotateY(window.getInput().getMouseX() / 800);
        window.getInput().setCursorPosition(0, 0);
		
        window.drawEnd();   
	}

	return 0;
}