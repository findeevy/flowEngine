#include "oglMesh.h"
#include "../bindings.h"
#include "oglShader.h"

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
}

static int OGLMesh::lodKey(const glm::ivec3 &g) {
  return (g.x & 0x3FF) | ((g.y & 0x3FF) << 10) | ((g.z & 0x3FF) << 20);
}

void OGLMesh::lodGen(glm::ivec3 subdivisions, bool cache) {
  const glm::ivec3 grid = glm::max(subdivisions, glm::ivec3(1));
  const int key = lodKey(grid);

  const GLsizeiptr clusterCount = (GLsizeiptr)grid.x * grid.y * grid.z;

  GLuint clusterSSBO =
      makeSSBO(clusterCount * (GLsizeiptr)sizeof(lodQuadric), GL_DYNAMIC_COPY);
  GLuint optRepSSBO =
      makeSSBO(clusterCount * (GLsizeiptr)sizeof(glm::vec4), GL_DYNAMIC_COPY);

  OGLLod lod;
  lod.vertexSSBO =
      makeSSBO(clusterCount * (GLsizeiptr)sizeof(Vertex), GL_DYNAMIC_COPY);
  lod.indexSSBO = makeSSBO((GLsizeiptr)(indices.size() * sizeof(uint32_t)),
                           GL_DYNAMIC_COPY);

  static OGLShader quadricPass({"glsl/compute/lodQuadric.comp"});

  zeroFill(clusterSSBO);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_VERTICES, VBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_SRC_INDICES, EBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LOD_CLUSTERS, clusterSSBO);

  const glm::vec3 extent = glm::max(maxaabb - minaabb, glm::vec3(1e-6f));
  const glm::vec3 cellSize = extent / glm::vec3(grid);
  const int triangleCount = (int)(indices.size() / 3);

  quadricPass.bind();

  glUniform3i(glGetUniformLocation(quadricPass.id, "uGrid"), grid.x, grid.y,
              grid.z);
  quadricPass.setVec3("uAabbMin", minaabb);
  quadricPass.setVec3("uCellSize", cellSize);
  quadricPass.setInt("uTriangleCount", triangleCount);

  glDispatchCompute((GLuint)((triangleCount + 63) / 64), 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // Computation of optimal representative positions

  // Decimated mesh generation
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