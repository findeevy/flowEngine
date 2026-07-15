#pragma once

#include "vertex.h"
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>
#include <tiny_obj_loader.h>
#include <vector>

class Mesh {
public:
  Mesh(const std::string filePath);
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  virtual void draw() = 0;
  virtual ~Mesh() = default;

private:
  void computeTangents();
};
