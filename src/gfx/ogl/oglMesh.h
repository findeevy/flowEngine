#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <string>

#include <unordered_map>

#include <algorithm>

#include <cmath>

#include <cstdint>

#include <cstring>

#include "../mesh.h"
#include "oglShader.h"

struct OGLProbTree {
  bool valid = false;
  int split = 0;
  int maxDepth = 0; 
  int reqDepth = -1;
  GLuint keys = 0;  
  GLuint nodes = 0;  
  GLuint opt = 0; 
  uint32_t totalSlots = 0;
  uint32_t levels = 0;
  uint32_t offset[16] = {};
  uint32_t mask[16] = {};
  uint32_t res[16] = {};
  uint32_t direct[16] = {};
  float prob[16] = {}; // per-level participation probability
};

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

  // grid based
  void lodGridGen(int split, bool cache) override;
  // oct tree based coming soooon?
  void lodOctGen(int split, int maxDepth, float errFraction,
                 bool cache) override;

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

  void adoptLod(OGLLod &lod, bool owned);
  static void freeLod(OGLLod &lod);

  void buildProbTree(int split, int maxDepth);
  void freeProbTree();
  OGLProbTree probTree;

  OGLLod current;
  bool currentOwned = false;
  GLsizei drawCount = 0;
  static constexpr size_t kMaxOctCache = 16;

  // maybe add function or something that will cull cache beyond a certain point
  // or remove equal OGLLods? Maybe even pregen a certain set (say 128,64,32,16)
  // or just have it take an in that is 2^x? with a cap at like at like 10?
  // maybe wipe them if u gen another lol
  std::unordered_map<int, OGLLod> gridLods;
  std::unordered_map<uint64_t, OGLLod> octLods;
};
