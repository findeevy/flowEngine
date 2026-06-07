#include "gameObject.h"

GameObject::GameObject(const std::string &_name, std::shared_ptr<Mesh> _mesh,
                       const Material &_material, const glm::vec3 &_position,
                       const glm::quat &_rotation)
    : name(_name), mesh(std::move(_mesh)),
      material(std::make_shared<Material>(_material)), position(_position),
      rotation(_rotation) {}
