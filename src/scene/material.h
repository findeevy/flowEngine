#pragma once

#include <glm/vec3.hpp>

class Material {
public:
  GLuint diffuseMap = 0;
  GLuint specularMap = 0;
  float specularScalar = 1.0f;
  float diffuseScalar = 1.0f;
}
