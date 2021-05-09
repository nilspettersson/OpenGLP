#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

    };
}