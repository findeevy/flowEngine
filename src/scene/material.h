#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <memory>

#include "texture.h"

class Material {
public:
  std::shared_ptr<Texture> diffuseMap;
  std::shared_ptr<Texture> specularMap;
  glm::vec3 diffuseTint;
  glm::vec3 specularTint;

  Material(const std::string &diffusePath, const std::string &specularPath, const glm::vec3 &_diffuseTint, const glm::vec3 &_specularTint);
};
