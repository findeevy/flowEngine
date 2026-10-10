#include "oglShader.h"

// recursive include for glsl shaders, great for modularization :)
std::string OGLShader::loadShader(const std::string &path,
                                  const std::string &root,
                                  std::set<std::string> &included) {
  if (included.count(path))
    return "";
  included.insert(path);

  std::ifstream file(path);
  if (!file)
    LOG_EVENT("Can't open shader file: " + path);

  size_t slash = path.find_last_of("/\\");
  std::string dir =
      (slash == std::string::npos) ? "" : path.substr(0, slash + 1);

  std::string result, line;
  while (std::getline(file, line)) {
    size_t start = line.find_first_not_of(" \t");
    if (start != std::string::npos && line.compare(start, 8, "#include") == 0) {
      size_t q1 = line.find('"', start);
      size_t q2 = line.find('"', q1 + 1);
      if (q1 == std::string::npos || q2 == std::string::npos)
        LOG_EVENT("Bad #include in " + path + " on line " + line);

      std::string inc = line.substr(q1 + 1, q2 - q1 - 1);
      std::string incPath = (inc[0] == '/') ? root + inc : dir + inc;

      result += loadShader(incPath, root, included);
      result += "\n";
    } else {
      result += line + "\n";
    }
  }
  return result;
}

OGLShader::OGLShader(const std::vector<std::string> &shaderPaths) {
  id = glCreateProgram();
  std::vector<unsigned> shaders;

  for (size_t i = 0; i < shaderPaths.size(); i++) {
    std::string shaderPath = shaderPaths[i];
    std::string shaderStr;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      size_t slash = shaderPath.find_last_of("/\\");
      std::string root =
          (slash == std::string::npos) ? "" : shaderPath.substr(0, slash);
      std::set<std::string> included;
      shaderStr = loadShader(shaderPath, root, included);
    } catch (const std::exception &e) {
      LOG_EVENT(e.what());
      continue;
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
    } else if (extension == "glsl") {
      LOG_EVENT(shaderPath +
                " is marked glsl, this should be used as a called function (or "
                "set of functions) in another shader, not used directly.");
      return;
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

  for (size_t i = 0; i < shaders.size(); i++) {
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
