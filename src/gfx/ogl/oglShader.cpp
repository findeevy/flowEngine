#include "oglShader.h"

OGLShader::OGLShader(const std::vector<std::string> &shaderPaths) {
  id = glCreateProgram();
  std::vector<unsigned> shaders;

  for (int i = 0; i < shaderPaths.size(); i++) {
    std::string shaderPath = shaderPaths[i];
    std::string shaderStr;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      shaderFile.open(shaderPath);

      std::stringstream shaderStream;
      shaderStream << shaderFile.rdbuf();

      shaderStr = shaderStream.str();
    } catch (const std::ifstream::failure &e) {
      LOG_EVENT(e.what());
    }

    const char *shaderCode = shaderStr.c_str();
    std::string extension =
        shaderPath.substr(std::max(0, (int)shaderPath.length() - 4));
    GLenum shaderType = 0;
    const char *shaderName = nullptr;

    if (extension == "vert") {
      shaderType = GL_VERTEX_SHADER;
      shaderName = "VERTEX";
    } else if (extension == "frag") {
      shaderType = GL_FRAGMENT_SHADER;
      shaderName = "FRAGMENT";
    } else if (extension == "geom") {
      shaderType = GL_GEOMETRY_SHADER;
      shaderName = "GEOMETRY";
    } else if (extension == "comp") {
      shaderType = GL_COMPUTE_SHADER;
      shaderName = "COMPUTE";
    } else if (extension == "ctrl") {
      shaderType = GL_TESS_CONTROL_SHADER;
      shaderName = "CONTROL";
    } else if (extension == "eval") {
      shaderType = GL_TESS_EVALUATION_SHADER;
      shaderName = "EVALUATION";
    } else {
      LOG_EVENT(shaderPath + " is an unknown type.");
      return;
    }

    unsigned int shader = glCreateShader(shaderType);
    shaders.emplace_back(shader);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, shaderName);

    glAttachShader(id, shader);
  }

  glLinkProgram(id);
  checkCompileErrors(id, "PROGRAM");

  for (int i = 0; i < shaders.size(); i++) {
    glDeleteShader(shaders[i]);
  }
}

void OGLShader::setBool(const std::string &name, bool value) {
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void OGLShader::setInt(const std::string &name, int value) {
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void OGLShader::setFloat(const std::string &name, float value) {
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void OGLShader::setVec2(const std::string &name, const glm::vec2 &value) {
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void OGLShader::setVec3(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void OGLShader::setVec4(const std::string &name, const glm::vec4 &value) {
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void OGLShader::setMat2(const std::string &name, const glm::mat2 &value) {
  glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void OGLShader::setMat3(const std::string &name, const glm::mat3 &value) {
  glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void OGLShader::setMat4(const std::string &name, const glm::mat4 &value) {
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
