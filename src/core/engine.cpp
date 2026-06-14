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

  auto diffuse = renderer->createTexture("assets/Albedo.jpg");
  auto specular = renderer->createTexture("assets/Specular.jpg");
  auto normal = renderer->createTexture("assets/Normal.jpg");

  std::map<std::string, std::shared_ptr<Texture>> textures = {
      {"diffuseMap", diffuse},
      {"specularMap", specular},
      {"normalMap", normal},
  };

  std::map<std::string, UniformValue> uniforms = {
      {"diffuseTint", glm::vec3(1.0f)},
      {"specularTint", glm::vec3(1.0f)},
      {"emissionTint", glm::vec3(0.0f)},
      {"hasDiffuse", true},
      {"hasSpecular", true},
      {"hasEmission", false},
      {"hasNormal", true},
  };

  Material mat(*renderer,
               {"shaders/glsl/base.vert", "shaders/glsl/bayerDither.frag"},
               textures, uniforms);

  Material matp(*renderer,
                {"shaders/glsl/base.vert", "shaders/glsl/blinnPhong.frag"},
                textures, uniforms);

  auto mesh = renderer->createMesh("assets/Barrel.obj");
  GameObject barrel("test", mesh, mat, {-1.0f, -1.0f, -30.0f},
                    glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, -90.0f))));

  GameObject barr("testu", mesh, matp, {-5.0f, -1.0f, -30.0f},
                  glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, -90.0f))));

  PointLight light;
  light.color = {1.0f, 1.0f, 1.0f, 50.0f};
  light.position = {2.0f, 2.0f, 0.0f};

  DirectionalLight dirLight;
  dirLight.color = {1.0f, 0.0f, 0.0f, 40.0f};
  dirLight.direction = {-2.0f, -1.0f, -2.0f};

  Camera cam;
  cam.position = {0.0f, 0.0f, 0.0f};
  cam.rotation = glm::quat(1, 0, 0, 0);

  scene = Scene(cam, {barrel, barr}, {light}, {dirLight});

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
