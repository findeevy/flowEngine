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
  window.initWindow(1920, 1080, "FlowEngine");

  renderer = Render::create(RendererBackend::OpenGL);

  gui.init(window.getWindowPtr());

  LOG_EVENT("FlowEngine started");

  // TEST CODE
  auto albedo = renderer->createTexture("assets/albedo.jpg");

  auto normal = renderer->createTexture("assets/normal.png");

  auto metallic = renderer->createTexture("assets/metallic.png");

  auto roughness = renderer->createTexture("assets/roughness.png");

  auto ao = renderer->createTexture("assets/ao.png");

  std::shared_ptr<Texture> emissive;

  std::map<std::string, std::shared_ptr<Texture>> textures = {
      {"albedoMap", albedo},
      {"normalMap", normal},
      {"metallicMap", metallic},
      {"roughnessMap", roughness},
      {"aoMap", ao},
  };

  std::map<std::string, UniformValue> uniforms = {
      {"albedoTint", glm::vec3(1.0f)},
      {"emissiveTint", glm::vec3(0.0f)},

      {"metallicFactor", 1.0f},
      {"roughnessFactor", 1.0f},

      {"hasAlbedo", true},
      {"hasNormal", true},
      {"hasMetallic", true},
      {"hasRoughness", true},
      {"hasAO", true},
      {"hasEmissive", false},

      {"ambientLight", glm::vec3(0.03f)},
  };

  Material mat(*renderer,
               {
                   "shaders/glsl/base.vert",
                   "shaders/glsl/base.frag",
               },
               textures, uniforms);

  auto mesh = renderer->createMesh("assets/head.obj");

  std::vector<GameObject> objs;

  for (float p = -10.0f; p < 10.0f; p++) {
    for (float i = -10.0f; i < 10.0f; i++) {
      for (float j = -10.0f; j < 10.0f; j++) {
        objs.emplace_back(GameObject(
            "head", mesh, mat, {i / 2.0f, j / 2.0f, (p / 2.0f) - 5.5f},
            glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, -90.0f)))));
      }
    }
  }

  PointLight light;
  light.color = {1.0f, 1.0f, 1.0f, 30.0f};
  light.position = {1.0f, 0.0f, 1.0f};

  DirectionalLight dirLight;

  SpotLight spotLight;

  Camera cam;
  cam.position = {0.0f, 0.0f, 0.0f};
  cam.rotation = glm::quat(1, 0, 0, 0);

  scene = Scene(cam, objs, {light}, {dirLight}, {spotLight});
  // TEST CODE

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
