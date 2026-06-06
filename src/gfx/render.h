#pragma once

#include "../scene/scene.h"
#include <glm/vec4.hpp>

class Render {
public:
  glm::vec4 clearColor{0.0f, 0.0f, 0.0f, 1.0f};

  // i dont think ill need an init, just include in instructor?
  Render() = default;
  virtual ~Render() = default;
  virtual void draw(const Scene &scene) = 0;
};
