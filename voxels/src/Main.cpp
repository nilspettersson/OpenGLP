#include "OpenGLP.h"
#include "./Terrain/ChunkGenerator.h"
#include "./Terrain/ChunkManager.h"

int main(void) {

	auto window = glp::Window(/*720, 480*/);
	auto camera = glp::Camera3d(window);
	camera.setZ(-3);
    camera.setY(-40);

	auto renderer = glp::Renderer(camera);


    auto shader = glp::Shader("res/shaders/main.shader");
    auto texture = glp::Texture("res/textures/test2.png", glp::Texture::FILTER::LINEAR);
    
    /*auto chunk = ChunkGenerator(0, 0, 16, 16);
    auto vao = glp::Vao(chunk.generateMesh(), false);
	auto entity = glp::Entity(vao, &shader, 1);
    entity.addTexture(&texture);*/



    auto chunckManager = ChunkManager(50, 16, 255);
    chunckManager.CreateEntities();

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);
	while (!window.shouldClose()) {
		window.drawStart();

		//renderer.render(entity);
        for (int i = 0; i < chunckManager.entities.size(); i++) {
            renderer.render(chunckManager.entities[i]);
        }


        float speed = 0.08;
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
