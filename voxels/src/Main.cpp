#include "OpenGLP.h"
#include "./Terrain/ChunkGenerator.h"
#include "./Terrain/ChunkManager.h"

using namespace std::chrono;
float getCurrentTime() {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now.time_since_epoch());
    float seconds = duration.count() / 1000.0f;
    return fmod(seconds, 3600.0f); // Wrap around every hour
}

int main(void) {

    auto window = glp::Window(1200, 900);
	auto camera = glp::Camera3d(window);
	camera.setZ(-3);
    camera.setY(-126);

    auto renderer = glp::Renderer(camera);

    int chunkSize = 128;
    auto chunckManager = new ChunkManager(14, chunkSize, 384);

    bool useCursorMovement = true;

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);


    float startTime = std::chrono::system_clock::now().time_since_epoch().count();

	while (!window.shouldClose()) {
        glm::vec3 skyColor = { 0.5, 0.7, 1 };
        /*glm::vec3 sunDirection = glm::vec3(cos(getCurrentTime()), sin(getCurrentTime()), 0.0f);
        float sunIntensity = 1.0f;
        if (sunDirection.y > 0) {
            skyColor *= 1.01f - sunDirection.y / 1.2f;
        }*/
        
		window.drawStart(skyColor.r, skyColor.g, skyColor.b);

        auto currentTime = std::chrono::system_clock::now();
        //auto seconds = std::chrono::duration<float>(currentTime.time_since_epoch()).count();
        float timeDifference = currentTime.time_since_epoch().count() - startTime;
        //std::cout << getCurrentTime() << std::endl;
        chunckManager->shader.setUniform1f("u_time", -0.25);

        chunckManager->shader.setUniform3f("u_camera", camera.getX(), camera.getY(), camera.getZ());

        chunckManager->updateChunks((camera.getX()) / (chunkSize), camera.getZ() / (chunkSize));
        chunckManager->originX = (camera.getX()) / (chunkSize);
        chunckManager->originZ = (camera.getZ()) / (chunkSize);

        for (auto i = chunckManager->chunks.begin(); i != chunckManager->chunks.end(); i++) {
            const auto &chunk = i->second;
            if (chunk->chunkEntity == nullptr) continue;
            renderer.render(*chunk->chunkEntity);
        }


        if (window.getInput().isKeyDown(GLP_KEY_DELETE)) {
            std::unique_lock<std::shared_mutex> lock(chunckManager->chunksMutex);
            for (auto i = chunckManager->chunks.begin(); i != chunckManager->chunks.end(); i++) {
                if (i->second != nullptr) {
                    delete i->second;
                }
            }
            chunckManager->chunks.clear();
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
