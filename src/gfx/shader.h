#pragma once

#include <glm/vec3.hpp>
#include <string>

class Shader {
public:
  virtual ~Shader() = default;
  virtual void bind() = 0;

  virtual void setInt(const std::string &name, int value) = 0;
  virtual void setFloat(const std::string &name, float value) = 0;
  virtual void setVec3(const std::string &name, const glm::vec3 &value) = 0;
};
