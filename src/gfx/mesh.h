#pragma once

#include "lodQuadric.h"
#include "vertex.h"
#include <array>
#include <glm/glm.hpp>
#include <limits>
#include <stdexcept>
#include <string>
#include <tiny_obj_loader.h>
#include <tuple>
#include <unordered_map>
#include <vector>

class Mesh {
public:
  Mesh(const std::string filePath);
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  glm::vec3 minaabb = glm::vec3(std::numeric_limits<float>::infinity());
  glm::vec3 maxaabb = glm::vec3(-std::numeric_limits<float>::infinity());

  // https://gfx.cs.princeton.edu/gfx/pubs/DeCoro_2007_RMS/real_time_simplification.pdf
  virtual void lodGen(glm::ivec3 subdivisions, bool cache) = 0;

  virtual void draw() = 0;
  virtual ~Mesh() = default;

private:
  void computeTangents();
};
