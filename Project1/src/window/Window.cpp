#include <iostream>
#include "wtypes.h"

#include "Window.h"

using namespace glp;

void getDesktopResolution(int& width, int& height) {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    width = desktop.right;
    height = desktop.bottom;
}

Window::Window(int width, int height) {
    this->window = NULL;
    this->width = width;
    this->height = height;

    if (glfwInit()) {
        this->window = glfwCreateWindow(width, height, "window", NULL, NULL);
        if (!this->window) {
            destroy();
        }

        glfwMakeContextCurrent(this->window);

        if (glewInit() != GLEW_OK) {
            std::cout << "glew error!" << std::endl;
        }
    }

    this->input = Input(this->window);
}

Window::Window() {
    this->window = NULL;
    this->width = 1920;
    this->height = 1080;
    getDesktopResolution(this->width, this->height);

    if (glfwInit()) {
        this->window = glfwCreateWindow(this->width, this->height, "window", glfwGetPrimaryMonitor(), NULL);
        if (!this->window) {
            destroy();
        }

        glfwMakeContextCurrent(this->window);

        if (glewInit() != GLEW_OK) {
            std::cout << "glew error!" << std::endl;
        }
    }

    this->input = Input(this->window);
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

Input Window::getInput() {
    return this->input;
}