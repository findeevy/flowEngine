#include "particleSystem.h"

ParticleSystem::ParticleSystem(
    const std::string &_name, const Material &_material, const float _lifeSpan,
    const int _maxAmount, const int _spawnPerSec const glm::vec3 &_position,
    const glm::quat &_rotation)
    : name(_name), material(std::make_shared<Material>(_material)),
      lifeSpan(_lifeSpan), maxAmount(_maxAmount), spawnPerSec(_spawnPerSec),
      position(_position), rotation(_rotation) {}

ParticleSystem::update(const float &dt) {
  if (isUpdating)
    return;

  elapsedTime += dt;
  totalSinceSpawn += dt;

  if (isEmitting && totalSinceSpawn >= (1.0f / spawnPerSec)) {
    for (int i = (int)(totalSinceSpawn * spawnPerSec); 0 < i; i--) {
      particles.push(Particle(elapsedTime, material));
    }
    totalSinceSpawn = 0.0f;
  }

  while ((particles.front().spawnTime + lifeSpanSec) < elapsedTime ||
         particles.size() > maxAmount) {
    particles.pop();
  }
}

ParticleSystem::play() {
  isEmitting = true;
  isUpdating = true;
}

ParticleSystem::pause() {
  isEmitting = false;
  isUpdating = false;
}

ParticleSystem::stop() { isEmitting = false; }

ParticleSystem::clear() {
  std::queue<Particle> empty;
  std::swap(particles, empty);
}
