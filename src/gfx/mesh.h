#pragma once

#include "tinyobjloader.h"
#include "vertex.h"
#include <glm/vec3.hpp>
#include <string>
#include <vector>

class Mesh {
public:
  Mesh(const std::string filePath) {
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

        vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                           1.0f -
                               attrib.texcoords[2 * index.texcoord_index + 1]};

        if (uniqueVertices.count(vertex) == 0) {
          uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
          vertices.emplace_back(vertex);
        }

        indices.emplace_back(uniqueVertices[vertex]);
      }
    }
  }

private:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
};
