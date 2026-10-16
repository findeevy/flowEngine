#include "engine.h"
#include "../utils/logger.h"

void FlowEngine::start() {
  init();
  while (!window.shouldClose()) {
    update();
  }
  shutdown();
}

void FlowEngine::init() {
  Project gameData = loadProject("example/game.univ");
  window.initWindow(gameData.width, gameData.height, gameData.name.c_str());

  renderer = Render::create(RendererBackend::OpenGL);

  gui.init(window.getWindowPtr());

  LOG_EVENT("FlowEngine started");

  try {
    scene = loadScene(gameData.startScene, *renderer);
    LOG_EVENT("Loaded level");
  } catch (const std::exception &e) {
    LOG_EVENT(std::string("Failed to load level: ") + e.what());
    throw;
  }

  network.detectHostIp();

  lastFrameTime = std::chrono::steady_clock::now();
}

void FlowEngine::update() {
  auto now = std::chrono::steady_clock::now();
  deltaTime = std::chrono::duration<float>(now - lastFrameTime).count();
  lastFrameTime = now;
  deltaTime = std::min(deltaTime, 0.5f);
  renderer->draw(scene);

  // TEST CODE
  angle += 60.0f * deltaTime;
  for (GameObject &object : scene.gameObjects) {
    object.rotation = glm::quat(glm::radians(glm::vec3(0.0f, angle, 0.0f)));
  }
  // TEST CODE

  gui.draw();
  window.swapBuffers();
  window.pollInput();
  updateQueue(gui.poll());

  process();

  // TEST CODE
  std::cout << "\r" << 1.0f / deltaTime << std::flush;
  // TEST CODE
}

void FlowEngine::updateQueue(std::vector<Event> events) {
  eventQueue.insert(eventQueue.end(), std::make_move_iterator(events.begin()),
                    std::make_move_iterator(events.end()));
}

void FlowEngine::process() {
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
  scene.update(deltaTime);
  eventQueue.clear();
}

void FlowEngine::shutdown() {
  gui.shutdown();
  window.closeWindow();
  LOG_EVENT("FlowEngine closed");
}
