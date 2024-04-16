#include "OpenGLP.h"
#include "./Terrain/ChunkGenerator.h"
#include "./Terrain/ChunkManager.h"

int main(void) {

    auto window = glp::Window(1000, 800);
	auto camera = glp::Camera3d(window);
	camera.setZ(-3);
    camera.setY(-126);

	auto renderer = glp::Renderer(camera);

    int chunkSize = 16;
    auto chunckManager = new ChunkManager(32, chunkSize, 256);

    bool useCursorMovement = true;

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);

	while (!window.shouldClose()) {
		window.drawStart(0.5, 0.7, 1);

        chunckManager->updateChunks((camera.getX()) / (chunkSize), camera.getZ() / (chunkSize));
        chunckManager->originX = (camera.getX()) / (chunkSize);
        chunckManager->originZ = (camera.getZ()) / (chunkSize);

        //std::shared_lock<std::shared_mutex> chunkListLock(chunckManager->chunksMutex);
        for (auto i = chunckManager->chunks.begin(); i != chunckManager->chunks.end(); i++) {
            auto chunk = i->second;
            //std::lock_guard<std::mutex> lock(chunk->chunkLock);
            if (chunk->chunkEntity == nullptr) continue;
            renderer.render(*chunk->chunkEntity);
        }
        //chunkListLock.unlock();

        if (window.getInput().isKeyDown(GLP_KEY_DELETE)) {
            std::unique_lock<std::shared_mutex> lock(chunckManager->chunksMutex);
            for (auto i = chunckManager->chunks.begin(); i != chunckManager->chunks.end(); i++) {
                //std::unique_lock<std::mutex> lock2(it->second->chunkLock);
                if (i->second != nullptr) {
                    delete i->second;
                }
                
                //lock2.unlock();
            }
            chunckManager->chunks.clear();
            lock.unlock();
        }


        float speed = 2;
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

        if (window.getInput().isKeyDown(GLP_KEY_C)) {
            useCursorMovement = true;
            window.getInput().setCursorDisabled(true);
            window.getInput().setCursorPosition(0, 0);
        }
        if (window.getInput().isKeyDown(GLP_KEY_V)) {
            useCursorMovement = false;
            window.getInput().setCursorNormal();
        }
        if (useCursorMovement) {
            camera.rotateX(window.getInput().getMouseY() / 800);
            camera.rotateY(window.getInput().getMouseX() / 800);
            window.getInput().setCursorPosition(0, 0);
        }

		window.drawEnd();
	}

    delete chunckManager;

}
