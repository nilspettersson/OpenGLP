#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Input.h"

namespace glp {
    class Window {
        GLFWwindow* window;
        int width;
        int height;

    public:
        Window();
        Window(int width, int height);

        bool shouldClose();
        GLFWwindow* getWindow();
        void destroy();
        void drawInit();
        void clean();

        bool isKeyDown(int);

    };
}