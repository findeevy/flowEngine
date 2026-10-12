#include "scene.h"
#include "../utils/logger.h"
#include <glm/gtc/constants.hpp>

Scene::Scene(const Camera &_camera, const std::vector<GameObject> &_gameObjects,
             const std::vector<PointLight> &_pointLights,
             const std::vector<DirectionalLight> &_directionalLights,
             const std::vector<SpotLight> &_spotLights)
    : camera(_camera), gameObjects(_gameObjects), pointLights(_pointLights),
      directionalLights(_directionalLights), spotLights(_spotLights) {}

void Scene::update(float dt) {
  // iterate through scene objects update/init code, delete, add.
}
