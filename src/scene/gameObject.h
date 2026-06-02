#pragma once

#include "material.h"
#include "mesh.h"

#include <glm/quat.hpp>
#include <glm/vec3.hpp>
#include <cstdint>
#include <string>
#include <vector>

class GameObject {
public:
  GameObject(const std::string &objectName, const std::string &meshFilePath,
             const Material &objectMaterial, const glm::vec3 &objectPosition,
             const glm::quat &objectRotation) {
    name = objectName;
    mesh = Mesh(meshFilePath);
    material = objectMaterial;
    position = objectPosition;
    rotation = objectRotation;
  }

  Material material;
  Mesh mesh;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  uint32 uid = 0;
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  std::string name;
};
