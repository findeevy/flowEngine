#pragma once

#include <glad/gl.h>
#include <glm/glm.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "../../utils/logger.h"
#include "../shader.h"

class OGLShader : public Shader {
public:
  unsigned int id;

  OGLShader(const std::string &vertexPath, const std::string &fragmentPath);
  void bind();

  void setBool(const std::string &name, int value);
  void setInt(const std::string &name, int value);
  void setFloat(const std::string &name, float value);

  void setVec2(const std::string &name, const glm::vec2 &value);
  void setVec3(const std::string &name, const glm::vec3 &value);
  void setVec4(const std::string &name, const glm::vec4 &value);

  void setMat2(const std::string &name, const glm::mat2 &value);
  void setMat3(const std::string &name, const glm::mat3 &value);
  void setMat4(const std::string &name, const glm::mat4 &value);

private:
  void checkCompileErrors(GLuint shader, std::string type);
};
