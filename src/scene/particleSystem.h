#pragma once

#include "material.h"
#include "particle.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <queue>
#include <string>

class ParticleSystem {
public:
  ParticleSystem(const std::string &_name, const Material &_material,
                 const float &_lifeSpan, const size_t &_maxAmount,
                 const unsigned &_spawnPerSec, const glm::vec3 &_position,
                 const glm::quat &_rotation);
  // should run every frame
  void update(const float &dt);

  std::string name = "";
  Material material;
  float lifeSpanSec = 10.0f;
  size_t maxAmount = 100;
  unsigned spawnPerSec = 10;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

  // controls from the outside
  void start();
  void stop();
  void pause();
  void clear();

private:
  double elapsedTime = 0.0f;

  // particle made up of material + init time to keep track of lifespan?
  std::queue<Particle> particles;
};
