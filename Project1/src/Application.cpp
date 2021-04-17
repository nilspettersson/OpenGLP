#include <iostream>
#include <crtdbg.h>
#include "Window.h"
#include "shader/Shader.h"
#include <fstream>
#include <string>
#include <sstream>

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Window window = Window(600, 600);

    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f,
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    Shader::ShaderProgramSouce source = Shader::parseShader("res/shaders/main.shader");
    Shader shader(source.vertexSource, source.fragmentSource);

    while (!window.shouldClose()) {
        window.drawInit();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.clean();
    }
    window.destroy();

    return 0;
};


