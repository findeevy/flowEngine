#include "gameObject.h"

GameObject::GameObject(const std::string &_name,
                       const std::string &meshFilePath,
                       const Material &_material, const glm::vec3 &_position,
                       const glm::quat &_rotation)
    : name(_name), mesh(std::make_shared<Mesh>(meshFilePath)),
      material(std::make_shared<Material>(_material)), position(_position),
      rotation(_rotation) {}
