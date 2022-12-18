#include "OpenGLP.h"

int main(void) {
    glp::Window window = glp::Window();
    glp::Camera3d camera = glp::Camera3d(1920, 1080);
    camera.setZ(-1000);

    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        -0.5f + 1.0f, -0.5f, 0.0f, 0.0f, 2.0f,
        0.5f + 1.0f, -0.5f, 1.0f, 0.0f, 2.0f,
        0.5f + 1.0f, 0.5f, 1.0f, 1.0f, 2.0f,
        -0.5f + 1.0f, 0.5f, 0.0f, 1.0f, 2.0f
    };
    std::vector<unsigned int> indices {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4
    };

    //glp::Mesh mesh = glp::Mesh::loadModelFromObj("res/models/test.obj");
    glp::Mesh mesh = glp::Mesh({ 2, 2, 1 }, vertices, indices);
    glp::Vao vao(mesh, false);
    glp::Shader shader("res/shaders/main.shader");
    glp::Texture texture("res/textures/test.png", glp::Texture::FILTER::NEAREST);
    glp::Texture texture2("res/textures/test2.png", glp::Texture::FILTER::NEAREST);

    std::vector<glp::Entity> entites = std::vector<glp::Entity>();
    for (int i = 0; i < 10; i++) {
        for (int ii = 0; ii < 10; ii++) {
            entites.push_back(glp::Entity(vao, &shader, 100));
            entites[entites.size() - 1].addTexture(&texture);
            entites[entites.size() - 1].addTexture(&texture2);

            entites[entites.size() - 1].setX(i * 300 - 460);
            entites[entites.size() - 1].setY(ii * 300 - 460);

            glp::Entity child(vao, &shader, 0.4f);
            child.addTexture(&texture);
            child.addTexture(&texture2);
            child.setY(140);
            entites[entites.size() - 1].addChild(child);
        }
    }
    glp::Renderer renderer = glp::Renderer(camera);

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);

    while (!window.shouldClose()) {
        window.drawStart();
        {
            //util::Timer timer;
            for (int i = 0; i < entites.size(); i++) {
                renderer.render(entites[i]);
            }
        }

        camera.rotateX((float)window.getInput().getMouseY() / 400);
        camera.rotateY((float)window.getInput().getMouseX() / 400);

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

        window.getInput().setCursorPosition(0, 0);
        window.drawEnd();
    }

    window.destroy();
    return 0;
};