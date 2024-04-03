#include "OpenGLP.h"
#include "./Terrain/ChunkGenerator.h"
#include "./Terrain/ChunkManager.h"

int main(void) {

    auto window = glp::Window(720, 480);
	auto camera = glp::Camera3d(window);
	camera.setZ(-3);
    camera.setY(-40);

	auto renderer = glp::Renderer(camera);

    int chunkSize = 16;
    auto chunckManager = ChunkManager(16, chunkSize, 255);
    chunckManager.CreateEntities();

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);
	while (!window.shouldClose()) {
		window.drawStart();

        chunckManager.updateChunks((camera.getX()) / (chunkSize), camera.getZ() / (chunkSize));

        for (int i = 0; i < chunckManager.entities.size(); i++) {
            renderer.render(*chunckManager.entities[i]);
        }


        float speed = 0.2;
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

}
