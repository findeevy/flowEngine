#pragma once

#include <glm/vec3.hpp>

#include "texture.h"

class Material {
public:
  Texture diffuseMap;
  Texture specularMap;
  float specularScalar = 1.0f;
  float diffuseScalar = 1.0f;
}
