#pragma once

#include <glm/vec3.hpp>

class Mesh {
public:
  // location
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};
