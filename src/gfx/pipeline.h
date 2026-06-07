#pragma once

#include <glm/vec3.hpp>
#include <string>

class Pipeline {
public:
  virtual ~Pipeline() = default;
  virtual void bind() = 0;
};
