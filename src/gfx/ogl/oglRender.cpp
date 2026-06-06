#include "../../utils/logger.h"
#include "../render.h"
#include <GLFW/glfw3.h>

OGLRender::OGLRender() {
  if (!gladLoadGL(glfwGetProcAddress)) {
    LOG_EVENT("Failed to initialize GLAD");
    exit(0);
  }
}

void OGLRender::draw(const Scene &_scene) {
  glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto &object : _scene.gameObjects) {
    // draw
  }
}
