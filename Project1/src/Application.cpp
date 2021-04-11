#include <iostream>
#include <crtdbg.h>
#include "Window.h"


int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Window window = Window();

    while (!window.shouldClose()) {
        window.drawInit();

        glBegin(GL_TRIANGLES);
        glVertex2d(-0.5f, -0.5f);
        glVertex2d(0.0f, 0.5f);
        glVertex2d(0.5f, -0.5f);
        glEnd();
        
        window.clean();
    }
    window.destroy();

    return 0;
};


