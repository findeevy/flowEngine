#pragma once

#include "material.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <queue>
#include <string>

class ParticleSystem {
public:
  ParticleSystem(const std::string &_name, const Material &_material,
                 const float lifeSpan const glm::vec3 &_position,
                 const glm::quat &_rotation);
  // should run every frame
  void update();

  std::string name = "";
  Material material;
  float lifeSpanSec = 10.0f;
  int maxAmount = 100;
  int spawnPerSec 10;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

  // controls from the outside
  void start();
  void stop();
  void pause();
  void clear();

private:
  std::queue<Particle> particles;
};
