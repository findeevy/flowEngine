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

  auto albedo =
      renderer->createTexture("assets/Vintage_Wooden_Barrel_4k_Albedo.jpg");

  auto normal = renderer->createTexture(
      "assets/Vintage_Wooden_Barrel_4k_Normal_OpenGL.jpg");

  auto metallic =
      renderer->createTexture("assets/Vintage_Wooden_Barrel_4k_Metallic.jpg");

  auto roughness =
      renderer->createTexture("assets/Vintage_Wooden_Barrel_4k_Roughness.jpg");

  auto ao = renderer->createTexture("assets/Vintage_Wooden_Barrel_4k_AO.jpg");

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
                   "shaders/glsl/cookTorrancePBR.frag",
               },
               textures, uniforms);

  auto mesh = renderer->createMesh("assets/Vintage_Wooden_Barrel.obj");

  GameObject barrel("Barrel", mesh, mat, {0.0f, 0.0f, 0.0f},
                    glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, -90.0f))));

  PointLight light;
  light.color = {1.0f, 1.0f, 1.0f, 50.0f};
  light.position = {-1.0f, -1.0f, 0.0f};

  DirectionalLight dirLight;

  SpotLight spotLight;

  Camera cam;
  cam.position = {0.0f, 0.0f, 0.0f};
  cam.rotation = glm::quat(1, 0, 0, 0);

  scene = Scene(cam, {barrel}, {light}, {dirLight}, {spotLight});

  network.detectHostIp();
}

void FlowEngine::update() {
  renderer->draw(scene);

  angle += 0.4f;
  scene.gameObjects[0].rotation =
      glm::quat(glm::radians(glm::vec3(angle, angle, -90.0f)));

  gui.draw();
  window.swapBuffers();
  window.pollInput();
  updateQueue(gui.poll());

  process();
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
  eventQueue.clear();
}

void FlowEngine::shutdown() {
  gui.shutdown();
  window.closeWindow();
  LOG_EVENT("FlowEngine closed");
}
