#pragma once

#include "camera.h"
#include "directionalLight.h"
#include "gameObject.h"
#include "pointLight.h"
#include "spotLight.h"

#include <nlohmann/json.hpp>

#include <vector>
#include <string>

class Scene {
public:
  Scene() = default;

  //default constructor
  Scene(const Camera &_camera, const std::vector<GameObject> &_gameObjects,
        const std::vector<PointLight> &_pointLights,
        const std::vector<DirectionalLight> &_directionalLights,
        const std::vector<SpotLight> &_spotLights);
  
  //load from file
  Scene(const std::string &filePath);
  
  Camera camera;
  std::vector<GameObject> gameObjects;
  std::vector<PointLight> pointLights;
  std::vector<DirectionalLight> directionalLights;
  std::vector<SpotLight> spotLights;

  void update(float dt);
};
