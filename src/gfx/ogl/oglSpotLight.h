#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
struct OGLSpotLight {
  glm::vec4 color;
  glm::vec3 direction;
  float cutoff;
};
