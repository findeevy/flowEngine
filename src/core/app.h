#pragma once

#include <vector>

#include "../events/events.h"
#include "../gfx/render.h"
#include "../gui/gui.h"
#include "../net/network.h"
#include "window.h"

class FlowEngine {
public:
  void start();

private:
  guiInstance gui;
  windowInstance window;
  glInstance renderer;
  networkManager network;

  std::vector<Event> eventQueue;

  void init();
  void shutdown();
  void update();
  void process();

  void updateQueue(std::vector<Event> events);
};
