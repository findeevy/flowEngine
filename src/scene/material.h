#pragma once

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "../gfx/pipeline.h"
#include "../gfx/texture.h"

using UniformValue = std::variant<int, float, bool, glm::vec2, glm::vec3,
                                  glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

class Render;

class Material {
public:
  std::shared_ptr<Pipeline> pipeline;
  std::map<std::string, std::shared_ptr<Texture>> textures;
  std::map<std::string, UniformValue> uniforms;

  Material(Render &renderer, const std::vector<std::string> &shaderPaths,
           const std::map<std::string, std::shared_ptr<Texture>> &texturesInput,
           const std::map<std::string, UniformValue> &uniforms);
};
