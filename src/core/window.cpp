#include "window.h"
#include "../utils/logger.h"
#include <glad/gl.h>

void windowInstance::initWindow(size_t width, size_t height,
                                const char *title) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  windowPtr = glfwCreateWindow(width, height, title, NULL, NULL);
  if (windowPtr == NULL) {
    LOG_EVENT("Failed to create GLFW window");
    glfwTerminate();
    exit(0);
  }
  glfwMakeContextCurrent(windowPtr);
  glfwSetFramebufferSizeCallback(windowPtr, framebufferSizeCallback);
}

void windowInstance::swapBuffers() { glfwSwapBuffers(windowPtr); }

bool windowInstance::shouldClose() { return glfwWindowShouldClose(windowPtr); }

void windowInstance::pollInput() { glfwPollEvents(); }

void windowInstance::closeWindow() { glfwTerminate(); }

void windowInstance::framebufferSizeCallback(GLFWwindow *window, int width,
                                             int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow *windowInstance::getWindowPtr() { return windowPtr; }
