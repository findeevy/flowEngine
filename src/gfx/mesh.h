#pragma once

#include "tinyobjloader.h"
#include "vertex.h"
#include <glm/vec3.hpp>
#include <string>
#include <vector>

class Mesh {
public:
  Mesh(const std::string filePath);
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
};
