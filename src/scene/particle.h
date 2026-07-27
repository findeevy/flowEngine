#pragma once

#include "material"

class Particle {
public:
  Particle(const float &_spawnTime, const Material &_material)
      : spawnTime(_spawnTime), material(_material);
  float spawnTime;
  Material material;
};
