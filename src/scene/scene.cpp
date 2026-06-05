#include "scene.h"

Scene::Scene(Camera &_camera, std::vector<GameObject> &_gameObjects,
             std::vector<PointLight> &_pointLights)
    : camera(_camera), gameObjects(_gameObjects), pointLights(_pointLights)

{}
