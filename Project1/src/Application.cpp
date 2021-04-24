#include <iostream>
#include <crtdbg.h>
#include "Window.h"
#include "shader/Shader.h"
#include <fstream>
#include <string>
#include <sstream>
#include "model/Vao.h"

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Window window = Window(600, 600);

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

    while (!window.shouldClose()) {
        window.drawInit();

        vao.render();

        window.clean();
    }
    window.destroy();

    return 0;
};


