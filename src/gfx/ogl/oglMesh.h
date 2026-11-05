#pragma once

#include <glad/gl.h>

#include <string>

#include "../mesh.h"

struct OGLLod {
  GLuint vertexSSBO = 0;
  GLuint indexSSBO = 0;
  GLuint indexCount = 0;
};

class OGLMesh : public Mesh {
public:
  OGLMesh(const std::string filePath);
  void draw() override;
  void lodGen(glm::ivec3 subdivisions, bool cache) override;
  GLuint getVBO() const { return VBO; }
  GLuint getEBO() const { return EBO; }
  ~OGLMesh() override;

  OGLMesh(const OGLMesh &) = delete;
  OGLMesh &operator=(const OGLMesh &) = delete;

private:
  // vertex array object
  GLuint VAO;
  // vertex buffer object
  GLuint VBO;
  // element buffer object
  GLuint EBO;
  std::unordered_map<int, OGLLod> lods;
};
