#include <iostream>
#include "wtypes.h"
#include "Window.h"


void getDesktopResolution(int& width, int& height) {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    width = desktop.right;
    height = desktop.bottom;
}

Window::Window(int width, int height) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    this->width = width;
    this->height = height;

    if (glfwInit()) {
        this->window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
        if (!this->window) {
            destroy();
        }

        glfwMakeContextCurrent(this->window);

        if (glewInit() != GLEW_OK) {
            std::cout << "glew error!" << std::endl;
        }
    }

}

Window::Window() {
    this->width = 192;
    this->height = 108;
    getDesktopResolution(this->width, this->height);

    if (glfwInit()) {
        this->window = glfwCreateWindow(this->width, this->height, "Hello World", glfwGetPrimaryMonitor(), NULL);
        if (!this->window) {
            destroy();
        }

        glfwMakeContextCurrent(this->window);

        if (glewInit() != GLEW_OK) {
            std::cout << "glew error!" << std::endl;
        }
    }

}

bool Window::shouldClose() {
    return glfwWindowShouldClose(this->window);
}

GLFWwindow* Window::getWindow() {
    return this->window;
}

void Window::destroy() {
    glfwTerminate();
}

void Window::drawInit() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::clean() {
    glfwSwapBuffers(this->window);
    glfwPollEvents();
}
