#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../utils/logger.h"
#include "../shader.h"

class OGLShader : public Shader {
public:
  unsigned int id;

  OGLShader(const std::vector<std::string> &shaderPaths);
  void bind() override;

  void setBool(const std::string &name, bool value) override;
  void setInt(const std::string &name, int value) override;
  void setFloat(const std::string &name, float value) override;

  void setVec2(const std::string &name, const glm::vec2 &value) override;
  void setVec3(const std::string &name, const glm::vec3 &value) override;
  void setVec4(const std::string &name, const glm::vec4 &value) override;

  void setMat2(const std::string &name, const glm::mat2 &value) override;
  void setMat3(const std::string &name, const glm::mat3 &value) override;
  void setMat4(const std::string &name, const glm::mat4 &value) override;

private:
  void checkCompileErrors(GLuint shader, std::string type);
};
