#pragma once

#include "camera.h"
#include "directionalLight.h"
#include "gameObject.h"
#include "pointLight.h"
#include "spotLight.h"

#include <vector>

class Scene {
public:
  Scene() = default;
  Scene(const Camera &_camera, const std::vector<GameObject> &_gameObjects,
        const std::vector<PointLight> &_pointLights,
        const std::vector<DirectionalLight> &_directionalLights,
        const std::vector<SpotLight> &_spotLights);
  Camera camera;
  std::vector<GameObject> gameObjects;
  std::vector<PointLight> pointLights;
  std::vector<DirectionalLight> directionalLights;
  std::vector<SpotLight> spotLights;
};
