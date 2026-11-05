#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <string>

#include <unordered_map>

#include "../mesh.h"
#include "oglShader.h"

struct OGLLod {
  GLuint vao = 0;
  GLuint vertexSSBO = 0;
  GLuint indexSSBO = 0;
  GLuint indexCount = 0;
};

class OGLMesh : public Mesh {
public:
  OGLMesh(const std::string filePath);
  void draw() override;

  //grid based
  void lodGen(int split, bool cache) override;
  //oct tree based coming soooon?
  
  GLuint getVBO() const { return VBO; }
  GLuint getEBO() const { return EBO; }
  ~OGLMesh() override;

  OGLMesh(const OGLMesh &) = delete;
  OGLMesh &operator=(const OGLMesh &) = delete;

  int lodKey(const glm::ivec3 &g);

private:
  // vertex array object
  GLuint VAO;
  // vertex buffer object
  GLuint VBO;
  // element buffer object
  GLuint EBO;

  // maybe add function or something that will cull cache beyond a certain point
  // or remove equal OGLLods? Maybe even pregen a certain set (say 128,64,32,16)
  // or just have it take an in that is 2^x? with a cap at like at like 10?
  std::unordered_map<int, OGLLod> lods;
};
