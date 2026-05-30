#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

class windowInstance {
private:
  GLFWwindow *windowPtr;
  static void framebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);

public:
  void initWindow(size_t width, size_t height, const char *title);
  GLFWwindow *getWindowPtr();
  void pollInput();
  void swapBuffers();
  void closeWindow();
  bool shouldClose();
};
