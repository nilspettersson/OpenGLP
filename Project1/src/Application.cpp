#include <iostream>
#include <crtdbg.h>
#include <fstream>
#include <string>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model/Vao.h"
#include "Window.h"
#include "shader/Shader.h"
#include "Camera.h"

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Window window = Window(600, 600);
    glp::Camera2d camera = glp::Camera2d(600, 600);
    camera.setX(-200);
    camera.setY(-200);

    float positions[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f,
    };
    unsigned int indices[]{
        0, 1, 2,
        2, 3, 0
    };
    glp::Vao vao({ 2 }, indices, 6, positions, 4);

    glp::Shader shader("res/shaders/main.shader");
    shader.setUniform4f("color", 0.0, 1.0, 0.0, 1.0);

    shader.setUniformMat4f("u_mvp", camera.getProjection());
    
    while (!window.shouldClose()) {
        window.drawInit();

        vao.render();

        window.clean();
    }
    window.destroy();

    return 0;
};


