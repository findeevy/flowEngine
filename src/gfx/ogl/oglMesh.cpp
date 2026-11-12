#include "oglMesh.h"
#include "../bindings.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

OGLMesh::OGLMesh(const std::string filePath) : Mesh(filePath) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoord));
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tangent));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);

  OGLLod baseLod;
  baseLod.vao = VAO;
  baseLod.vertexSSBO = VBO;
  baseLod.indexSSBO = EBO;
  baseLod.indexCount = static_cast<uint32_t>(indices.size());

  gridLods[1] = baseLod;
  current = baseLod;
  currentOwned = false;
  drawCount = (GLsizei)indices.size();
}

int OGLMesh::lodKey(const glm::ivec3 &g) {
  return (g.x & 0x3FF) | ((g.y & 0x3FF) << 10) | ((g.z & 0x3FF) << 20);
}

void OGLMesh::freeLod(OGLLod &lod) {
  if (lod.vao)
    glDeleteVertexArrays(1, &lod.vao);
  if (lod.vertexSSBO)
    glDeleteBuffers(1, &lod.vertexSSBO);
  if (lod.indexSSBO)
    glDeleteBuffers(1, &lod.indexSSBO);
  lod = OGLLod{};
}

void OGLMesh::adoptLod(OGLLod &lod, bool owned) {
  if (currentOwned && current.vao != lod.vao)
    freeLod(current);

  if (lod.vao == 0) {
    glGenVertexArrays(1, &lod.vao);
    glBindVertexArray(lod.vao);
    glBindBuffer(GL_ARRAY_BUFFER, lod.vertexSSBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lod.indexSSBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
  }

  current = lod;
  currentOwned = owned;
  VAO = lod.vao;
  VBO = lod.vertexSSBO;
  EBO = lod.indexSSBO;
  drawCount = (GLsizei)lod.indexCount;
}

// split is the cuts per axis, cache is if we cache the result or not
void OGLMesh::lodGridGen(int split, bool cache) {
  OGLLod lod;
  bool fresh = false;
  if (gridLods.find(split) == gridLods.end()) {
    fresh = true;
    LOG_EVENT(std::to_string(split));
    const OGLLod base = gridLods.at(1);
    const glm::ivec3 grid = glm::max(glm::ivec3(split), glm::ivec3(1));

    const GLsizeiptr clusterCount = (GLsizeiptr)grid.x * grid.y * grid.z;

    const int triangleCount = (int)(base.indexCount / 3);

    const glm::vec3 extent = glm::max(maxaabb - minaabb, glm::vec3(1e-6f));
    const glm::vec3 cellSize = extent / glm::vec3(grid);

    const bool dedupe = true;

    GLuint clusterSSBO = OGLShader::makeSSBO(
        clusterCount * (GLsizeiptr)sizeof(lodQuadric), GL_DYNAMIC_COPY);
    GLuint optRepSSBO = OGLShader::makeSSBO(
        clusterCount * (GLsizeiptr)sizeof(Vertex), GL_DYNAMIC_COPY);
    GLuint counterSSBO = OGLShader::makeSSBO(sizeof(uint32_t), GL_DYNAMIC_COPY);
    GLuint claimedSSBO = OGLShader::makeSSBO(
        clusterCount * 2 * (GLsizeiptr)sizeof(uint32_t), GL_DYNAMIC_COPY);
    GLuint tableSSBO = 0;
    uint32_t tableSize = 0;
    if (dedupe) {
      tableSize = 1;
      while (tableSize < (uint32_t)triangleCount * 2u)
        tableSize <<= 1;
      tableSSBO = OGLShader::makeSSBO((GLsizeiptr)tableSize * sizeof(uint32_t),
                                      GL_DYNAMIC_COPY);
    }

    lod.vertexSSBO = OGLShader::makeSSBO(
        clusterCount * (GLsizeiptr)sizeof(Vertex), GL_DYNAMIC_COPY);
    lod.indexSSBO = OGLShader::makeSSBO(
        (GLsizeiptr)(base.indexCount * sizeof(uint32_t)), GL_DYNAMIC_COPY);

    const uint32_t zero = 0u, empty = 0xFFFFFFFFu;
    OGLShader::zeroFill(clusterSSBO);
    glClearNamedBufferData(counterSSBO, GL_R32UI, GL_RED_INTEGER,
                           GL_UNSIGNED_INT, &zero);
    glClearNamedBufferData(claimedSSBO, GL_R32UI, GL_RED_INTEGER,
                           GL_UNSIGNED_INT, &empty);
    if (dedupe)
      glClearNamedBufferData(tableSSBO, GL_R32UI, GL_RED_INTEGER,
                             GL_UNSIGNED_INT, &empty);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT |
                    GL_SHADER_STORAGE_BARRIER_BIT);

    static OGLShader quadricPass(
        {"shaders/glsl/compute/lod/lodGridQuadric.comp"});
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_VERTICES,
                     base.vertexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_INDICES, base.indexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_CLUSTERS, clusterSSBO);

    quadricPass.bind();
    glUniform3i(glGetUniformLocation(quadricPass.id, "uGrid"), grid.x, grid.y,
                grid.z);
    quadricPass.setVec3("uAabbMin", minaabb);
    quadricPass.setVec3("uCellSize", cellSize);
    quadricPass.setInt("uTriangleCount", triangleCount);
    glDispatchCompute((GLuint)((triangleCount + 63) / 64), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Computation of optimal representative positions
    static OGLShader optPass({"shaders/glsl/compute/lod/lodGridOptimal.comp"});
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OPT_REP, optRepSSBO);

    optPass.bind();
    glUniform3i(glGetUniformLocation(optPass.id, "uGrid"), grid.x, grid.y,
                grid.z);
    optPass.setVec3("uAabbMin", minaabb);
    optPass.setVec3("uCellSize", cellSize);
    glDispatchCompute((GLuint)(((int)clusterCount + 63) / 64), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Decimated mesh generation
    // for every triangle, check if it shares verts in any cell (delete if so)
    // if they're all in seperate then hash then check if already existing  and
    // put to claimed
    static OGLShader decimatePass(
        {"shaders/glsl/compute/lod/lodGridDecimate.comp"});
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OUT_VERTICES,
                     lod.vertexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OUT_INDICES, lod.indexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_COUNTER, counterSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_CLAIMED, claimedSSBO);
    if (dedupe)
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_HASH, tableSSBO);

    decimatePass.bind();
    glUniform3i(glGetUniformLocation(decimatePass.id, "uGrid"), grid.x, grid.y,
                grid.z);
    decimatePass.setVec3("uAabbMin", minaabb);
    decimatePass.setVec3("uCellSize", cellSize);
    decimatePass.setInt("uTriangleCount", triangleCount);
    if (dedupe)
      glUniform1ui(glGetUniformLocation(decimatePass.id, "uTableMask"),
                   tableSize - 1u);
    const GLuint groups = (GLuint)((triangleCount + 63) / 64);
    const GLint passLoc = glGetUniformLocation(decimatePass.id, "uPass");

    for (GLuint pass = 0; pass < 3; pass++) {
      glUniform1ui(passLoc, pass);
      glDispatchCompute(groups, 1, 1);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT |
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT);

    uint32_t outCount = 0;
    glGetNamedBufferSubData(counterSSBO, 0, sizeof(outCount), &outCount);
    lod.indexCount = outCount;

    glDeleteBuffers(1, &clusterSSBO);
    glDeleteBuffers(1, &optRepSSBO);
    glDeleteBuffers(1, &counterSSBO);
    glDeleteBuffers(1, &claimedSSBO);
    if (dedupe)
      glDeleteBuffers(1, &tableSSBO);

  } else {
    lod = gridLods[split];
  }

  adoptLod(lod, fresh && !cache);
  if (cache)
    gridLods[split] = lod;
}

namespace {
uint32_t nextPow2(uint64_t v) {
  uint64_t p = 1;
  while (p < v)
    p <<= 1;
  return (uint32_t)p;
}
}

void OGLMesh::freeProbTree() {
  if (probTree.keys)
    glDeleteBuffers(1, &probTree.keys);
  if (probTree.nodes)
    glDeleteBuffers(1, &probTree.nodes);
  if (probTree.opt)
    glDeleteBuffers(1, &probTree.opt);
  probTree = OGLProbTree{};
}

void OGLMesh::buildProbTree(int split, int maxDepth) {
  freeProbTree();

  const OGLLod base = gridLods.at(1);
  const uint32_t vertexCount = (uint32_t)vertices.size();
  const int triangleCount = (int)(base.indexCount / 3);

  probTree.reqDepth = maxDepth;

  const uint64_t kMaxRes = 1625; // 1625^3 < 2^32 - 1
  while (maxDepth > 0 && ((uint64_t)split << maxDepth) > kMaxRes)
    maxDepth--;

  OGLProbTree &t = probTree;
  t.split = split;
  t.maxDepth = maxDepth;
  t.levels = (uint32_t)maxDepth + 1;

  const double kSamplesPerNode = 32.0;
  t.prob[0] = 1.0f;
  for (int l = 1; l <= maxDepth; l++) {
    const double q = kSamplesPerNode * (double)split * (double)split *
                     std::pow(4.0, (double)l) /
                     std::max(1.0, (double)vertexCount);
    t.prob[l] = (float)std::min(1.0, q);
  }

  const double kLoadFine = 0.5, kLoadRoot = 0.25;
  uint64_t total = 0;
  for (uint32_t l = 0; l < t.levels; l++) {
    const uint64_t res = (uint64_t)split << l;
    const uint64_t cells = res * res * res;
    const double expected = (double)vertexCount * t.prob[l] * 1.2 + 64.0;
    const double load = (l == 0) ? kLoadRoot : kLoadFine;


    const uint64_t surfaceCap = 16ull * res * res;
    const uint64_t occupied =
        std::min<uint64_t>({cells, surfaceCap, (uint64_t)std::ceil(expected)});
    const uint32_t hashed =
        std::max<uint32_t>(64u, nextPow2((uint64_t)std::ceil(occupied / load)));

    t.res[l] = (uint32_t)res;
    t.offset[l] = (uint32_t)total;
    if (cells <= hashed) {
      t.direct[l] = 1;
      t.mask[l] = 0;
      total += cells;
    } else {
      t.direct[l] = 0;
      t.mask[l] = hashed - 1u;
      total += hashed;
    }
  }
  t.totalSlots = (uint32_t)std::min<uint64_t>(total, 0xFFFFFFF0ull);

  t.keys = OGLShader::makeSSBO((GLsizeiptr)t.totalSlots * sizeof(uint32_t),
                               GL_DYNAMIC_COPY);
  t.nodes = OGLShader::makeSSBO((GLsizeiptr)t.totalSlots * 56, GL_DYNAMIC_COPY);
  t.opt = OGLShader::makeSSBO((GLsizeiptr)t.totalSlots * sizeof(glm::vec4),
                              GL_DYNAMIC_COPY);
  GLuint vqSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)vertexCount * 10 * sizeof(float), GL_DYNAMIC_COPY);

  const uint32_t empty = 0xFFFFFFFFu;
  glClearNamedBufferData(t.keys, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
                         &empty);
  OGLShader::zeroFill(t.nodes);
  OGLShader::zeroFill(vqSSBO);
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  const glm::vec3 extent = glm::max(maxaabb - minaabb, glm::vec3(1e-6f));
  const glm::vec3 cellSize0 = extent / glm::vec3((float)split);

  static OGLShader quadricPass(
      {"shaders/glsl/compute/lod/lodOctProbQuadric.comp"});
  static OGLShader insertPass(
      {"shaders/glsl/compute/lod/lodOctProbInsert.comp"});
  static OGLShader optPass({"shaders/glsl/compute/lod/lodOctProbOptimal.comp"});

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_VERTICES, base.vertexSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_INDICES, base.indexSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_CLUSTERS, t.nodes);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OPT_REP, t.opt);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OCT_KEYS, t.keys);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_VERT_QUADRIC, vqSSBO);

  auto setTreeUniforms = [&](OGLShader &sh) {
    glUniform1ui(glGetUniformLocation(sh.id, "uLevels"), t.levels);
    glUniform1uiv(glGetUniformLocation(sh.id, "uLevelOffset"), 16, t.offset);
    glUniform1uiv(glGetUniformLocation(sh.id, "uLevelMask"), 16, t.mask);
    glUniform1uiv(glGetUniformLocation(sh.id, "uLevelRes"), 16, t.res);
    glUniform1uiv(glGetUniformLocation(sh.id, "uLevelDirect"), 16, t.direct);
    sh.setVec3("uCellSize0", cellSize0);
  };

  quadricPass.bind();
  quadricPass.setVec3("uAabbMin", minaabb);
  quadricPass.setInt("uTriangleCount", triangleCount);
  glDispatchCompute((GLuint)((triangleCount + 63) / 64), 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  insertPass.bind();
  setTreeUniforms(insertPass);
  insertPass.setVec3("uAabbMin", minaabb);
  glUniform1ui(glGetUniformLocation(insertPass.id, "uVertexCount"),
               vertexCount);
  glUniform1ui(glGetUniformLocation(insertPass.id, "uSeed"), 0x9E3779B9u);
  glUniform1fv(glGetUniformLocation(insertPass.id, "uLevelProb"), 16, t.prob);
  glDispatchCompute((vertexCount + 63u) / 64u, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  optPass.bind();
  setTreeUniforms(optPass);
  glUniform1ui(glGetUniformLocation(optPass.id, "uTotalSlots"), t.totalSlots);
  glDispatchCompute((t.totalSlots + 63u) / 64u, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  glDeleteBuffers(1, &vqSSBO);
  t.valid = true;
}

void OGLMesh::lodOctGen(int split, int maxDepth, float errFraction,
                        bool cache) {
  maxDepth = std::clamp(maxDepth, 0, 15);
  split = std::clamp(split, 1, 1023);

  if (!(errFraction > 0.0f))
    errFraction = 0.0f;
  errFraction = std::round(errFraction * 1e6f) / 1e6f;
  errFraction = std::max(errFraction, 1e-6f);

  uint32_t errBits;
  std::memcpy(&errBits, &errFraction, sizeof(errBits));
  const uint64_t key =
      (uint64_t)split | ((uint64_t)maxDepth << 10) | ((uint64_t)errBits << 14);

  auto hit = octLods.find(key);
  if (hit != octLods.end()) {
    adoptLod(hit->second, false);
    return;
  }

  // The tree only depends on (split, maxDepth) ie changing the tolerance reuses
  // it. (maxDepth may be reduced inside buildProbTree, so compare the request.)
  if (!probTree.valid || probTree.split != split ||
      probTree.reqDepth != maxDepth)
    buildProbTree(split, maxDepth);
  const OGLProbTree &t = probTree;

  OGLLod lod;
  const OGLLod base = gridLods.at(1);
  const uint32_t vertexCount = (uint32_t)vertices.size();
  const int triangleCount = (int)(base.indexCount / 3);

  const glm::vec3 extent = glm::max(maxaabb - minaabb, glm::vec3(1e-6f));
  const glm::vec3 cellSize0 = extent / glm::vec3((float)split);
  const float tol = errFraction * glm::length(extent);
  const float threshold = tol * tol;

  const GLuint vertGroups = (vertexCount + 63u) / 64u;
  const GLuint triGroups = (GLuint)((triangleCount + 63) / 64);
  const GLuint slotGroups = (t.totalSlots + 63u) / 64u;

  uint32_t tableSize = 1;
  while (tableSize < (uint32_t)triangleCount * 2u)
    tableSize <<= 1;

  const uint32_t zero = 0u, empty = 0xFFFFFFFFu;
  GLuint vertClusterSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)vertexCount * sizeof(uint32_t), GL_DYNAMIC_COPY);
  GLuint bestSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)t.totalSlots * 2 * sizeof(uint32_t), GL_DYNAMIC_COPY);
  GLuint remapSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)t.totalSlots * sizeof(uint32_t), GL_DYNAMIC_COPY);
  GLuint tableSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)tableSize * sizeof(uint32_t), GL_DYNAMIC_COPY);
  GLuint counterSSBO = OGLShader::makeSSBO(sizeof(uint32_t), GL_DYNAMIC_COPY);
  GLuint vcountSSBO = OGLShader::makeSSBO(sizeof(uint32_t), GL_DYNAMIC_COPY);
  // there can't be more output vertices than input vertices
  GLuint scratchVerts = OGLShader::makeSSBO(
      (GLsizeiptr)vertexCount * sizeof(Vertex), GL_DYNAMIC_COPY);
  GLuint scratchIndex = OGLShader::makeSSBO(
      (GLsizeiptr)(base.indexCount * sizeof(uint32_t)), GL_DYNAMIC_COPY);

  glClearNamedBufferData(bestSSBO, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
                         &empty);
  glClearNamedBufferData(remapSSBO, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
                         &empty);
  glClearNamedBufferData(tableSSBO, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
                         &empty);
  glClearNamedBufferData(counterSSBO, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
                         &zero);
  glClearNamedBufferData(vcountSSBO, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT,
                         &zero);
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  static OGLShader lookupPass(
      {"shaders/glsl/compute/lod/lodOctProbLookup.comp"});
  static OGLShader decimatePass(
      {"shaders/glsl/compute/lod/lodOctDecimate.comp"});

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_VERTICES, base.vertexSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_INDICES, base.indexSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OPT_REP, t.opt);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OCT_KEYS, t.keys);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_VERT_CLUSTER, vertClusterSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OUT_VERTICES, scratchVerts);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OUT_INDICES, scratchIndex);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_COUNTER, counterSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_CLAIMED, bestSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_HASH, tableSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_REMAP, remapSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_VERT_COUNTER, vcountSSBO);

  // FINDCLUSTER for every vertex at this tolerance
  lookupPass.bind();
  glUniform1ui(glGetUniformLocation(lookupPass.id, "uLevels"), t.levels);
  glUniform1uiv(glGetUniformLocation(lookupPass.id, "uLevelOffset"), 16,
                t.offset);
  glUniform1uiv(glGetUniformLocation(lookupPass.id, "uLevelMask"), 16, t.mask);
  glUniform1uiv(glGetUniformLocation(lookupPass.id, "uLevelRes"), 16, t.res);
  glUniform1uiv(glGetUniformLocation(lookupPass.id, "uLevelDirect"), 16,
                t.direct);
  lookupPass.setVec3("uCellSize0", cellSize0);
  lookupPass.setVec3("uAabbMin", minaabb);
  glUniform1ui(glGetUniformLocation(lookupPass.id, "uVertexCount"),
               vertexCount);
  glUniform1f(glGetUniformLocation(lookupPass.id, "uThreshold"), threshold);
  glDispatchCompute(vertGroups, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  decimatePass.bind();
  decimatePass.setVec3("uAabbMin", minaabb);
  glUniform1ui(glGetUniformLocation(decimatePass.id, "uVertexCount"),
               vertexCount);
  glUniform1ui(glGetUniformLocation(decimatePass.id, "uTriangleCount"),
               (uint32_t)triangleCount);
  glUniform1ui(glGetUniformLocation(decimatePass.id, "uClusterCount"),
               t.totalSlots);
  glUniform1ui(glGetUniformLocation(decimatePass.id, "uTableMask"),
               tableSize - 1u);
  const GLint passLoc = glGetUniformLocation(decimatePass.id, "uPass");

  const GLuint first3[3] = {vertGroups, vertGroups, triGroups};
  for (GLuint pass = 0; pass < 3; pass++) {
    glUniform1ui(passLoc, pass);
    glDispatchCompute(first3[pass], 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  // readback #1: how many indices got emitted
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
  uint32_t outCount = 0;
  glGetNamedBufferSubData(counterSSBO, 0, sizeof(outCount), &outCount);

  // 3: dense vertex per used cluster, 4: renumber the indices
  glUniform1ui(passLoc, 3);
  glDispatchCompute(slotGroups, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  if (outCount > 0) {
    glUniform1ui(glGetUniformLocation(decimatePass.id, "uIndexCount"),
                 outCount);
    glUniform1ui(passLoc, 4);
    glDispatchCompute((outCount + 63u) / 64u, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

  // readback #2: dense vertex count
  uint32_t outVerts = 0;
  glGetNamedBufferSubData(vcountSSBO, 0, sizeof(outVerts), &outVerts);

  // exact-size output buffers (the scratch ones are sized for the worst case)
  lod.indexCount = outCount;
  lod.vertexSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)std::max<uint32_t>(outVerts, 1u) * sizeof(Vertex),
      GL_STATIC_DRAW);
  lod.indexSSBO = OGLShader::makeSSBO(
      (GLsizeiptr)std::max<uint32_t>(outCount, 1u) * sizeof(uint32_t),
      GL_STATIC_DRAW);
  if (outVerts > 0)
    glCopyNamedBufferSubData(scratchVerts, lod.vertexSSBO, 0, 0,
                             (GLsizeiptr)outVerts * sizeof(Vertex));
  if (outCount > 0)
    glCopyNamedBufferSubData(scratchIndex, lod.indexSSBO, 0, 0,
                             (GLsizeiptr)outCount * sizeof(uint32_t));

  glDeleteBuffers(1, &vertClusterSSBO);
  glDeleteBuffers(1, &bestSSBO);
  glDeleteBuffers(1, &remapSSBO);
  glDeleteBuffers(1, &tableSSBO);
  glDeleteBuffers(1, &counterSSBO);
  glDeleteBuffers(1, &vcountSSBO);
  glDeleteBuffers(1, &scratchVerts);
  glDeleteBuffers(1, &scratchIndex);
  LOG_EVENT("oct split=" + std::to_string(split) + " depth=" +
            std::to_string(t.maxDepth) + " err=" + std::to_string(errFraction) +
            " verts=" + std::to_string(vertexCount) +
            " slots=" + std::to_string(t.totalSlots) + " outVerts=" +
            std::to_string(outVerts) + " outIdx=" + std::to_string(outCount));

  adoptLod(lod, !cache);
  if (cache) {
    while (octLods.size() >= kMaxOctCache) {
      auto victim = octLods.begin();
      while (victim != octLods.end() && victim->second.vao == current.vao)
        ++victim;
      if (victim == octLods.end())
        break;
      freeLod(victim->second);
      octLods.erase(victim);
    }
    octLods[key] = lod;
  }
}

void OGLMesh::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

OGLMesh::~OGLMesh() {
  freeProbTree();
  if (currentOwned)
    freeLod(current);
  for (auto &kv : octLods)
    freeLod(kv.second);
  for (auto &kv : gridLods)
    freeLod(kv.second); // includes the base mesh (key 1)
}
