#pragma once

#include <glad/gl.h>

#include <string>

#include "../mesh.h"

class OGLMesh : public Mesh {
  // vertex array object
  // vertex buffer object
  // element buffer object
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  OGLMesh(const std::string filePath);
  void draw();
  ~OGLMesh();
}
