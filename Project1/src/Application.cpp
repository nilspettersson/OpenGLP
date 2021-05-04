#include <iostream>
#include <crtdbg.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model/Vao.h"
#include "window/Window.h"
#include "shader/Shader.h"
#include "camera/Camera.h"
#include "texture/Texture.h";

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Window window = Window(1000, 1000);
    glp::Camera2d camera = glp::Camera2d(1, 1);
    camera.setX(0);
    camera.setY(0);

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
    shader.setUniform4f("color", 0.0, 1.0, 0.0, 1.0);

    shader.setUniformMat4f("u_mvp", camera.getProjection());

    glp::Texture texture("res/textures/test.png", glp::Texture::FILTER::NEAREST);
    texture.bind();
    shader.setUniform1i("u_texture", 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    while (!window.shouldClose()) {
        window.drawInit();

        vao.render();

        window.clean();
    }
    window.destroy();

    return 0;
};


