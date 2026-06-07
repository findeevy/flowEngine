#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
struct OGLPointLight {
  glm::vec4 color;
  glm::vec3 position;
  float _pad;
};
