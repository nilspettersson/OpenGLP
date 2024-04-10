#include "OpenGLP.h"
#include "./Terrain/ChunkGenerator.h"
#include "./Terrain/ChunkManager.h"

int main(void) {

    auto window = glp::Window(1200, 900);
	auto camera = glp::Camera3d(window);
	camera.setZ(-3);
    camera.setY(-120);

	auto renderer = glp::Renderer(camera);

    int chunkSize = 16;
    auto chunckManager = new ChunkManager(24, chunkSize, 255);

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);

	while (!window.shouldClose()) {
		window.drawStart();

        chunckManager->updateChunks((camera.getX()) / (chunkSize), camera.getZ() / (chunkSize));
        chunckManager->originX = (camera.getX()) / (chunkSize);
        chunckManager->originZ = (camera.getZ()) / (chunkSize);

        for (int i = 0; i < chunckManager->entities.size(); i++) {
            renderer.render(*chunckManager->entities[i]);
        }

        if (window.getInput().isKeyDown(GLP_KEY_DELETE)) {
            std::unique_lock<std::mutex> lock(chunckManager->chunksMutex);
            for (auto it = chunckManager->chunks.begin(); it != chunckManager->chunks.end(); ) {
                delete it->second;
                it = chunckManager->chunks.erase(it);
            }
            chunckManager->chunks.clear();
            for (int j = chunckManager->entities.size() - 1; j >= 0; j--) {
                delete chunckManager->entities[j];
                chunckManager->entities.erase(chunckManager->entities.begin() + j);
            }
            lock.unlock();
        }


        float speed = 0.4;
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

    delete chunckManager;

}
