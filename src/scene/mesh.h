#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "../gfx/vertex.h"

class Mesh {
public:
  std::vector<vertex> vertices;
  std::vector<unsigned int> indices;
};
