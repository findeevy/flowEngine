#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "mesh.h"

class GameObject {
public:
  Mesh mesh;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};
