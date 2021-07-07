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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    glp::Window window = glp::Window(1000, 1000);
    glp::Camera3d camera = glp::Camera3d(1000, 1000);
    camera.setZ(-1000);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f
    };
    unsigned int indices[]{
        0, 1, 2,
        2, 3, 0
    };
    glp::Vao vao({2,2}, indices, 6, vertices, 4, true);
    glp::Shader shader("res/shaders/main.shader");
    //glp::Texture texture("res/textures/test.png", glp::Texture::FILTER::NEAREST);

    std::vector<glp::Entity> entites = std::vector<glp::Entity>();
    for (int i = 0; i < 20; i++) {
        for (int ii = 0; ii < 20; ii++) {
            entites.push_back(glp::Entity(&vao, &shader, 60));
            //entites[entites.size() - 1].setTexture(&texture);
            entites[entites.size() - 1].getShader()->setUniform4f("color", 1.0, 1.0, 1.0, 1.0);
            entites[entites.size() - 1].getShader()->setUniform1i("u_texture", 0);
            entites[entites.size() - 1].setX(i * 80 - 460);
            entites[entites.size() - 1].setY(ii * 80 - 460);
        }
    }
    /*glp::Entity entity = glp::Entity(&vao, &shader, 10);
    entity.setTexture(&texture);
    entity.getShader()->setUniform1i("u_texture", 0);
    entity.getShader()->setUniform4f("color", 0.0, 1.0, 0.0, 1.0);*/
    
    glp::Renderer renderer = glp::Renderer(&camera);

    window.getInput().setCursorDisabled(true);
    window.getInput().setCursorPosition(0, 0);

    float vertices2[] = {
        -0.2f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f
    };

    vao.setVertices(sizeof(vertices2), vertices2);


    //texture.bind();
    while (!window.shouldClose()) {
        window.drawInit();
        {
            //util::Timer timer;
            for (int i = 0; i < entites.size(); i++) {
                entites[i].getMaterial()->addProperty("color", sin(i / 100.0f), 1, cos(i / 100.0f));
                renderer.render(&entites[i]);
            }
        }

        camera.rotateX((float)window.getInput().getMouseY() / 400);
        camera.rotateY((float)window.getInput().getMouseX() / 400);
        
        if (window.getInput().isKeyDown(GLFW_KEY_W)) {
            camera.moveForward(6);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_S)) {
            camera.moveBackward(6);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_A)) {
            camera.moveLeft(6);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_D)) {
            camera.moveRight(6);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_Q)) {
            camera.moveDown(6);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_E)) {
            camera.moveUp(6);
        }

        window.getInput().setCursorPosition(0, 0);
        window.clean();
    }

    window.destroy();
    return 0;
};