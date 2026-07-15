#include "mesh.h"

#include <unordered_map>

Mesh::Mesh(const std::string filePath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  std::string warn;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        filePath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};
      vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                         attrib.vertices[3 * index.vertex_index + 1],
                         attrib.vertices[3 * index.vertex_index + 2]};

      if (index.normal_index >= 0) {
        vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                         attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2]};
      } else {
        vertex.normal = {0.0f, 0.0f, 1.0f};
      }
      if (index.texcoord_index >= 0) {
        vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                           1.0f -
                               attrib.texcoords[2 * index.texcoord_index + 1]};
      } else {
        vertex.texCoord = {0.0f, 0.0f};
      }

      vertex.tangent = {0.0f, 0.0f, 0.0f};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.emplace_back(vertex);
      }

      indices.emplace_back(uniqueVertices[vertex]);
    }
  }

  computeTangents();
}

void Mesh::computeTangents() {
  std::vector<glm::vec3> tangentAccum(vertices.size(), glm::vec3(0.0f));

  for (size_t i = 0; i + 2 < indices.size(); i += 3) {
    Vertex &v0 = vertices[indices[i]];
    Vertex &v1 = vertices[indices[i + 1]];
    Vertex &v2 = vertices[indices[i + 2]];

    glm::vec3 edge1 = v1.position - v0.position;
    glm::vec3 edge2 = v2.position - v0.position;

    glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
    glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

    float denom = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
    float f = (denom != 0.0f) ? 1.0f / denom : 0.0f;

    glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);

    tangentAccum[indices[i]] += tangent;
    tangentAccum[indices[i + 1]] += tangent;
    tangentAccum[indices[i + 2]] += tangent;
  }

  for (size_t i = 0; i < vertices.size(); i++) {
    glm::vec3 t = tangentAccum[i];
    if (glm::length(t) > 1e-8f) {
      vertices[i].tangent = glm::normalize(t);
    } else {
      vertices[i].tangent = glm::vec3(1.0f, 0.0f, 0.0f);
    }
  }
}
