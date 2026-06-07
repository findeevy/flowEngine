#pragma once

#include "camera.h"
#include "gameObject.h"
#include "pointLight.h"

#include <vector>

class Scene {
public:
  Scene() = default;
  Scene(const Camera &_camera, const std::vector<GameObject> &_gameObjects,
        const std::vector<PointLight> &_pointLights);
  Camera camera;
  std::vector<GameObject> gameObjects;
  std::vector<PointLight> pointLights;
};
