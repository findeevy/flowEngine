#include "particleSystema.h"

ParticleSystem::ParticleSystem(
    const std::string &_name, const Material &_material, const float _lifeSpan,
    const int _maxAmount, const int _spawnPerSec const glm::vec3 &_position,
    const glm::quat &_rotation)
    : name(_name), material(std::make_shared<Material>(_material)),
      lifeSpan(_lifeSpan), maxAmount(_maxAmount), spawnPerSec(_spawnPerSec),
      position(_position), rotation(_rotation) {}
