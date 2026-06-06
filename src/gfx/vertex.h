#pragma once

#include <cstddef>
#include <functional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;

  bool operator==(const Vertex &other) const {
    return position == other.position && normal == other.normal &&
           texCoord == other.texCoord;
  }
};

template <> struct std::hash<Vertex> {
  // for lookup in map, hash generation
  size_t operator()(const Vertex &v) const {
    size_t seed = 0;
    seed ^= std::hash<float>{}(v.position.x);
    seed ^= std::hash<float>{}(v.position.y);
    seed ^= std::hash<float>{}(v.position.z);
    seed ^= std::hash<float>{}(v.normal.x);
    seed ^= std::hash<float>{}(v.normal.y);
    seed ^= std::hash<float>{}(v.normal.z);
    seed ^= std::hash<float>{}(v.texCoord.x);
    seed ^= std::hash<float>{}(v.texCoord.y);
    return seed;
  }
};
