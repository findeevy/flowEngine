#include "oglShader.h"

OGLShader::OGLShader(const std::string &vertexPath,
                     const std::string &fragmentPath) {

  std::ifstream vertexFile;
  std::ifstream fragmentFile;

  vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vertexFile.open(vertexPath);
    fragmentFile.open(fragmentPath);

    std::stringstream vertexStream;
    std::stringstream fragmentStream;

    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    std::string vertexStr = vertexStream.str();
    const char *vertexCode = vertexStr.c_str();
    LOG_EVENT(vertexCode);
    std::string fragmentStr = fragmentStream.str();
    const char *fragmentCode = fragmentStr.c_str();
    LOG_EVENT(fragmentCode);
  } catch (const std::ifstream::failure &e) {
    LOG_EVENT(e.what());
  }

  unsigned vertex;
  unsigned fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexCode, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentCode, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  // assign id to shader program
  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);
  checkCompileErrors(id, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void OGLShader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void OGLShader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void OGLShader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void OGLShader::setVec2(const std::string &name, const glm::vec2 &value) const {
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void OGLShader::setVec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void OGLShader::setVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void OGLShader::setVec3(const std::string &name, float x, float y,
                        float z) const {
  glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void OGLShader::setVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void OGLShader::setVec4(const std::string &name, float x, float y, float z,
                        float w) const {
  glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void OGLShader::setMat2(const std::string &name, const glm::mat2 &value) const {
  glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void OGLShader::setMat3(const std::string &name, const glm::mat3 &value) const {
  glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void OGLShader::setMat4(const std::string &name, const glm::mat4 &value) const {
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void OGLShader::bind() { glUseProgram(id); }

void OGLShader::checkCompileErrors(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      LOG_EVENT(infoLog);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      LOG_EVENT(infoLog);
    }
  }
}
