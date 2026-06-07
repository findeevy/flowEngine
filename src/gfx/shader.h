#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
  virtual ~Shader() = default;
  virtual void bind() = 0;

  virtual void setBool(const std::string &name, bool value) = 0;
  virtual void setInt(const std::string &name, int value) = 0;
  virtual void setFloat(const std::string &name, float value) = 0;

  virtual void setVec2(const std::string &name, const glm::vec2 &value) = 0;
  virtual void setVec3(const std::string &name, const glm::vec3 &value) = 0;
  virtual void setVec4(const std::string &name, const glm::vec4 &value) = 0;

  virtual void setMat2(const std::string &name, const glm::mat2 &value) = 0;
  virtual void setMat3(const std::string &name, const glm::mat3 &value) = 0;
  virtual void setMat4(const std::string &name, const glm::mat4 &value) = 0;
};
