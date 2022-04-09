#pragma once
#include "Input.h"

namespace glp {
    class Window {
        void* window;
        Input input;
        int width;
        int height;

    public:
        Window();
        Window(int width, int height);

        bool shouldClose();
        void* getWindow();
        void destroy();
        void drawStart();
        void drawStart(float r, float g, float b);
        void drawEnd();

        Input getInput();
        int getWidth();
        int getHeight();

    };
}