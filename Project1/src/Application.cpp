#include <iostream>
#include <vector>
#include <crtdbg.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model/Vao.h"
#include "window/Window.h"
#include "shader/Shader.h"
#include "camera/Camera.h"
#include "texture/Texture.h";
#include "rendering/Renderer.h"
#include "entity/Entity.h";
#include "util/Util.h"
#include "model/Mesh.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    glp::Window window = glp::Window(1000, 1000);
    glp::Camera3d camera = glp::Camera3d(1000, 1000);
    camera.setZ(-1000);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        -0.5f + 1.0f, -0.5f, 0.0f, 0.0f, 2.0f,
        0.5f + 1.0f, -0.5f, 1.0f, 0.0f, 2.0f,
        0.5f + 1.0f, 0.5f, 1.0f, 1.0f, 2.0f,
        -0.5f + 1.0f, 0.5f, 0.0f, 1.0f, 2.0f
    };
    unsigned int indices[]{
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4
    };

    glp::Mesh mesh = glp::Mesh::loadModelFromObj("res/models/test.obj");
    glp::Vao vao(mesh, false);
    glp::Shader shader("res/shaders/main.shader");
    glp::Texture texture("res/textures/test.png", glp::Texture::FILTER::NEAREST);
    glp::Texture texture2("res/textures/test2.png", glp::Texture::FILTER::NEAREST);

    std::vector<glp::Entity> entites = std::vector<glp::Entity>();
    for (int i = 0; i < 10; i++) {
        for (int ii = 0; ii < 10; ii++) {
            entites.push_back(glp::Entity(&vao, &shader, 60));
            entites[entites.size() - 1].addTexture(&texture);
            entites[entites.size() - 1].addTexture(&texture2);
            
            entites[entites.size() - 1].setX(i * 300 - 460);
            entites[entites.size() - 1].setY(ii * 300 - 460);

            glp::Entity child(&vao, &shader, 0.4f);
            child.addTexture(&texture);
            child.addTexture(&texture2);
            child.setY(140);
            entites[entites.size() - 1].addChild(child);
        }
    }
    glp::Renderer renderer = glp::Renderer(&camera);

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);

    while (!window.shouldClose()) {
        window.drawInit();
        {
            //util::Timer timer;
            for (int i = 0; i < entites.size(); i++) {
                renderer.render(&entites[i]);
            }
        }

        camera.rotateX((float)window.getInput().getMouseY() / 400);
        camera.rotateY((float)window.getInput().getMouseX() / 400);
        
        if (window.getInput().isKeyDown(GLFW_KEY_W)) {
            camera.moveForward(3);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_S)) {
            camera.moveBackward(3);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_A)) {
            camera.moveLeft(3);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_D)) {
            camera.moveRight(3);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_Q)) {
            camera.moveDown(3);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_E)) {
            camera.moveUp(3);
        }

        window.getInput().setCursorPosition(0, 0);
        window.clean();
    }

    window.destroy();
    return 0;
};