#include <iostream>
#include "wtypes.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

        glfwMakeContextCurrent((GLFWwindow*)this->window);

        if (glewInit() != GLEW_OK) {
            std::cout << "glew error!" << std::endl;
        }
    }

    glEnable(GL_DEPTH_TEST);
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

        glfwMakeContextCurrent((GLFWwindow*)this->window);

        if (glewInit() != GLEW_OK) {
            std::cout << "glew error!" << std::endl;
        }
    }

    glEnable(GL_DEPTH_TEST);
    this->input = Input(this->window);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose((GLFWwindow*)this->window);
}

void* Window::getWindow() {
    return (GLFWwindow*)this->window;
}

void Window::destroy() {
    glfwTerminate();
}

void Window::drawStart() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glp::Window::drawStart(float r, float g, float b) {
    glClearColor(r, g, b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::drawEnd() {
    glfwSwapBuffers((GLFWwindow*)this->window);
    glfwPollEvents();
}

Input Window::getInput() {
    return this->input;
}

int Window::getWidth() {
    return this->width;
}
int Window::getHeight() {
    return this->height;
}