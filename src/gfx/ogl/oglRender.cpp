#include "../utils/logger.h"
#include "render.h"
#include <GLFW/glfw3.h>

void glInstance::init() {
  if (!gladLoadGL(glfwGetProcAddress)) {
    LOG_EVENT("Failed to initialize GLAD");
    exit(0);
  }
}

void glInstance::draw() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}
