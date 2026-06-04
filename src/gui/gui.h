#pragma once

#include "../events/events.h"
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class guiInstance {
public:
  void init(GLFWwindow *window);
  void draw();
  void shutdown();
  std::vector<Event> poll();

private:
  bool showOptions = true;
  bool showAssets = true;
  bool shoFlowEngineder = true;

  std::vector<Event> guiQueue;

  void assetsPanel();
  void renderPanel();
  void optionsPanel();
};
