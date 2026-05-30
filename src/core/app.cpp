#include "app.h"
#include "../utils/logger.h"

void WREN::start() {
  init();
  while (!window.shouldClose()) {
    update();
  }
  shutdown();
}

void WREN::init() {
  window.initWindow(1280, 720, "WREN");
  renderer.init();
  gui.init(window.getWindowPtr());
  // network.init();
  LOG_EVENT("WREN started");
  network.detectHostIp();
}

void WREN::update() {
  renderer.draw();
  gui.draw();
  window.swapBuffers();
  window.pollInput();
  updateQueue(gui.poll());
  // updateQueue(network.poll());
  process();
}

void WREN::updateQueue(std::vector<Event> events) {
  eventQueue.insert(eventQueue.end(), std::make_move_iterator(events.begin()),
                    std::make_move_iterator(events.end()));
}

void WREN::process() {
  for (Event &event : eventQueue) {
    std::visit(
        [&](auto &&ev) {
          using T = std::decay_t<decltype(ev)>;
          if constexpr (std::is_same_v<T, EvSelectServerMode>) {
            LOG_EVENT("Server mode selected");
          } else if constexpr (std::is_same_v<T, EvSelectClientMode>) {
            LOG_EVENT("Client mode selected");
          }
        },
        event);
  }
  eventQueue.clear();
}

void WREN::shutdown() {
  gui.shutdown();
  window.closeWindow();
  LOG_EVENT("WREN closed");
}
