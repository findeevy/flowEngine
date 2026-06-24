#pragma once

#include <glm/vec3.hpp>

class SpotLight {
public:
  // rgb, intensity
  glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

  // location
  glm::vec3 direction{0.0f, 0.0f, 0.0f};
  float innerCutoff = 1.0f;

  glm::vec3 position{0.0f, 0.0f, 0.0f};

  float outerCutoff = 1.0f;

  float constant = 1.0f;
  float linear = 1.0f;
  float quadratic = 1.0f;
};
