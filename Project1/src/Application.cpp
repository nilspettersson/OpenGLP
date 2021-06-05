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

    glp::Window window = glp::Window(800, 800);
    glp::Camera2d camera = glp::Camera2d(800, 800);

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
    glp::Texture texture("res/textures/test.png", glp::Texture::FILTER::NEAREST);

    std::vector<glp::Entity> entites = std::vector<glp::Entity>();
    for (int i = 0; i < 100; i++) {
        for (int ii = 0; ii < 10; ii++) {
            entites.push_back(glp::Entity(&vao, &shader, 60));
            entites[entites.size() - 1].setTexture(&texture);
            entites[entites.size() - 1].getShader()->setUniform1i("u_texture", 0);
            entites[entites.size() - 1].getShader()->setUniform4f("color", 0.0, 1.0, 0.0, 1.0);
            entites[entites.size() - 1].setX(i * 80 - 460);
            entites[entites.size() - 1].setY(ii * 80 - 460);
        }
    }
    /*glp::Entity entity = glp::Entity(&vao, &shader, 10);
    entity.setTexture(&texture);
    entity.getShader()->setUniform1i("u_texture", 0);
    entity.getShader()->setUniform4f("color", 0.0, 1.0, 0.0, 1.0);*/
    
    glp::Renderer renderer = glp::Renderer(&camera);

    texture.bind();
    while (!window.shouldClose()) {
        window.drawInit();
        {
            //util::Timer timer;
            for (int i = 0; i < entites.size(); i++) {
                renderer.render(&entites[i]);
            }
        }
        //renderer.render(&entity);

        if (window.isKeyDown(GLFW_KEY_P)) {
            std::cout << "click" << std::endl;
        }

        window.clean();
    }

    window.destroy();
    return 0;
};