#pragma once

#include <glm/vec3.hpp>

class PointLight {
public:
  // rgb, intensity
  glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

  // location
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  float _pad{0.0f};
};
