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

    Vao vao({ 2 }, indices, positions);


    Shader shader("res/shaders/main.shader");

    while (!window.shouldClose()) {
        window.drawInit();

        glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, nullptr);

        window.clean();
    }
    window.destroy();

    return 0;
};


