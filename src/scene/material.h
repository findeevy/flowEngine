#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include <optional>
#include <string>

#include "../gfx/pipeline.h"
#include "../gfx/texture.h"

class Render;

class Material {
public:
  std::shared_ptr<Pipeline> pipeline;

  std::optional<std::shared_ptr<Texture>> diffuseMap;
  std::optional<std::shared_ptr<Texture>> specularMap;
  std::optional<std::shared_ptr<Texture>> emissionMap;

  glm::vec3 diffuseTint{1.0f, 1.0f, 1.0f};
  glm::vec3 specularTint{1.0f, 1.0f, 1.0f};
  glm::vec3 emissionTint{1.0f, 1.0f, 1.0f};

  std::optional<std::shared_ptr<Texture>> normalMap;

public:
  Material(Render &renderer, const std::vector<std::string> &shaderPaths,
           const std::string &diffusePath, const std::string &specularPath,
           const std::string &emissionPath, const std::string &normalPath,
           const glm::vec3 &diffuseTint = glm::vec3(1),
           const glm::vec3 &specularTint = glm::vec3(1),
           const glm::vec3 &emissionTint = glm::vec3(0));
};
