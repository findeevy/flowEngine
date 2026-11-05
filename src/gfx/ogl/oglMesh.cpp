#include "oglMesh.h"
#include "../bindings.h"

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

  lods[1] = baseLod;
}

int OGLMesh::lodKey(const glm::ivec3 &g) {
  return (g.x & 0x3FF) | ((g.y & 0x3FF) << 10) | ((g.z & 0x3FF) << 20);
}

void OGLMesh::lodGen(int split, bool cache) {
  OGLLod lod;
  if (lods.find(split) == lods.end()) {
    LOG_EVENT(std::to_string(split));
    // prolly a better way to do this tbh... preload OG to gen from
    lodGen(1, false);
    const glm::ivec3 grid = glm::max(glm::ivec3(split), glm::ivec3(1));
    const int key = lodKey(grid);

    const GLsizeiptr clusterCount = (GLsizeiptr)grid.x * grid.y * grid.z;

    const int triangleCount = (int)(indices.size() / 3);

    const glm::vec3 extent = glm::max(maxaabb - minaabb, glm::vec3(1e-6f));
    const glm::vec3 cellSize = extent / glm::vec3(grid);

    const bool dedupe = true;

    GLuint clusterSSBO = OGLShader::makeSSBO(
        clusterCount * (GLsizeiptr)sizeof(lodQuadric), GL_DYNAMIC_COPY);
    GLuint optRepSSBO = OGLShader::makeSSBO(
        clusterCount * (GLsizeiptr)sizeof(glm::vec4), GL_DYNAMIC_COPY);
    GLuint counterSSBO = OGLShader::makeSSBO(sizeof(uint32_t), GL_DYNAMIC_COPY);
    GLuint claimedSSBO = OGLShader::makeSSBO(
        clusterCount * (GLsizeiptr)sizeof(uint32_t), GL_DYNAMIC_COPY);
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
        (GLsizeiptr)(indices.size() * sizeof(uint32_t)), GL_DYNAMIC_COPY);

    const uint32_t zero = 0u, empty = 0xFFFFFFFFu;
    OGLShader::zeroFill(clusterSSBO);
    glClearNamedBufferData(counterSSBO, GL_R32UI, GL_RED_INTEGER,
                           GL_UNSIGNED_INT, &zero);
    glClearNamedBufferData(claimedSSBO, GL_R32UI, GL_RED_INTEGER,
                           GL_UNSIGNED_INT, &zero);
    if (dedupe)
      glClearNamedBufferData(tableSSBO, GL_R32UI, GL_RED_INTEGER,
                             GL_UNSIGNED_INT, &empty);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT |
                    GL_SHADER_STORAGE_BARRIER_BIT);

    static OGLShader quadricPass({"shaders/glsl/compute/lodQuadric.comp"});
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_VERTICES, VBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_INDICES, EBO);
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
    static OGLShader optPass({"shaders/glsl/compute/lodOptimal.comp"});
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_OPT_REP, optRepSSBO);

    optPass.bind();
    glUniform3i(glGetUniformLocation(optPass.id, "uGrid"), grid.x, grid.y,
                grid.z); // uniforms are per-program
    optPass.setVec3("uAabbMin", minaabb);
    optPass.setVec3("uCellSize", cellSize);
    glDispatchCompute((GLuint)(((int)clusterCount + 63) / 64), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Decimated mesh generation
    // for every triangle, check if it shares verts in any cell (delete if so)
    // if they're all in seperate then hash then check if already existing  and
    // put to claimed
    static OGLShader decimatePass({"shaders/glsl/compute/lodDecimate.comp"});
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
    glDispatchCompute((GLuint)((triangleCount + 63) / 64), 1, 1);
    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT |
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT);

    uint32_t outCount = 0;
    glGetNamedBufferSubData(counterSSBO, 0, sizeof(outCount), &outCount);
    lod.indexCount = outCount;

    if (cache)
      lods[split] = lod;

    glDeleteBuffers(1, &clusterSSBO);
    glDeleteBuffers(1, &optRepSSBO);
    glDeleteBuffers(1, &counterSSBO);
    glDeleteBuffers(1, &claimedSSBO);
    if (dedupe)
      glDeleteBuffers(1, &tableSSBO);

  } else {
    lod = lods[split];
  }

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
  VAO = lod.vao;
  VBO = lod.vertexSSBO;
  EBO = lod.indexSSBO;
  indices.resize(lod.indexCount);
}

void OGLMesh::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

OGLMesh::~OGLMesh() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}