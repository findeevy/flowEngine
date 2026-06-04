#pragma once

#include "../gfx/mesh.h"
#include "material.h"

#include <cstdint>
#include <glm/quat.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

class GameObject {
public:
  GameObject(const std::string &_name, const std::string &meshFilePath,
             const Material &_material, const glm::vec3 &_position,
             const glm::quat &_rotation);

  std::shared_ptr<Material> material;
  std::shared_ptr<Mesh> mesh;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  uint32_t uid = 0;
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  std::string name = "";
};
