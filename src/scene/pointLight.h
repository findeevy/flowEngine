#pragma once

#include <glm/vec3.hpp>

class PointLight {
public:
  // settings
  float fov = 1.0f;
  glm::vec3 color{0.0f, 0.0f, 0.0f};

  // location
  glm::vec3 position{0.0f, 0.0f, 0.0f};
};
