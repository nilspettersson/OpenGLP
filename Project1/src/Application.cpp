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

    glp::Window window = glp::Window(600, 600);
    glp::Camera3d camera = glp::Camera3d(600, 600);
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
    glp::Vao vao({2, 2}, indices, 6, vertices, 4);
    glp::Shader shader("res/shaders/main.shader");
    //glp::Texture texture("res/textures/test.png", glp::Texture::FILTER::NEAREST);

    std::vector<glp::Entity> entites = std::vector<glp::Entity>();
    for (int i = 0; i < 40; i++) {
        for (int ii = 0; ii < 40; ii++) {
            entites.push_back(glp::Entity(&vao, &shader, 60));
            //entites[entites.size() - 1].setTexture(&texture);
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

    float yy = 0;
    float xx = 0;

    //texture.bind();
    while (!window.shouldClose()) {
        window.drawInit();
        {
            util::Timer timer;
            for (int i = 0; i < entites.size(); i++) {
                entites[i].getShader()->setUniform4f("color", sin(i / 100.0f), 1, cos(i / 100.0f), 1.0);
                renderer.render(&entites[i]);
            }
        }
        //renderer.render(&entity);

        yy += (float)window.getInput().getMouseX() / 400;
        xx += (float)window.getInput().getMouseY() / 400;
        camera.rotateX((float)window.getInput().getMouseY() / 400);
        camera.rotateY((float)window.getInput().getMouseX() / 400);
        


        if (window.getInput().isKeyDown(GLFW_KEY_W)) {
            glm::vec3 pos = glm::rotate(glm::quat(camera.rotation), glm::vec3(0, 0, 6.0f));
            camera.setX(camera.getX() - pos.x);
            camera.setY(camera.getY() - pos.y);
            camera.setZ(camera.getZ() + pos.z);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_S)) {
            glm::vec3 pos = glm::rotate(glm::quat(camera.rotation), glm::vec3(0, 0, -6.0f));
            camera.setX(camera.getX() - pos.x);
            camera.setY(camera.getY() - pos.y);
            camera.setZ(camera.getZ() + pos.z);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_A)) {
            glm::vec3 pos = glm::rotate(glm::quat(camera.rotation), glm::vec3(-6.0f, 0, 0));
            camera.setX(camera.getX() - pos.x);
            camera.setY(camera.getY() - pos.y);
            camera.setZ(camera.getZ() + pos.z);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_D)) {
            glm::vec3 pos = glm::rotate(glm::quat(camera.rotation), glm::vec3(6.0f, 0, 0));
            camera.setX(camera.getX() - pos.x);
            camera.setY(camera.getY() - pos.y);
            camera.setZ(camera.getZ() + pos.z);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_Q)) {
            glm::vec3 pos = glm::rotate(glm::quat(camera.rotation), glm::vec3(0, -6.0f, 0));
            camera.setX(camera.getX() - pos.x);
            camera.setY(camera.getY() - pos.y);
            camera.setZ(camera.getZ() + pos.z);
        }
        if (window.getInput().isKeyDown(GLFW_KEY_E)) {
            glm::vec3 pos = glm::rotate(glm::quat(camera.rotation), glm::vec3(0, 6.0f, 0));
            camera.setX(camera.getX() - pos.x);
            camera.setY(camera.getY() - pos.y);
            camera.setZ(camera.getZ() + pos.z);
        }


        window.getInput().setCursorPosition(0, 0);
        window.clean();
    }

    window.destroy();
    return 0;
};