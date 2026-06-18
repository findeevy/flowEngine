#pragma once

#include <glm/vec3.hpp>

class SpotLight {
public:
  // rgb, intensity
  glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

  // location
  glm::vec3 direction{0.0f, 0.0f, 0.0f};
  float cutoff = 1.0f;
};
