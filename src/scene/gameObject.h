#pragma once

#include "../gfx/mesh.h"
#include "material.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>

class GameObject {
public:
  GameObject(const std::string &_name, std::shared_ptr<Mesh> _mesh,
             const Material &_material, const glm::vec3 &_position,
             const glm::quat &_rotation);

  std::string name = "";
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};
