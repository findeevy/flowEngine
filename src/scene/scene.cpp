#include "scene.h"

Scene::Scene(const Camera &_camera, const std::vector<GameObject> &_gameObjects,
             const std::vector<PointLight> &_pointLights)
    : camera(_camera), gameObjects(_gameObjects), pointLights(_pointLights) {}
