#pragma once

#include "../scene/scene.h"
#include <glm/vec4.hpp>
#include <memory>

enum class RendererBackend { OpenGL, Vulkan };

class Render {
public:
  static std::unique_ptr<Render> create(RendererBackend backend);
  glm::vec4 clearColor{0.0f, 0.0f, 0.0f, 1.0f};

  // i dont think ill need an init, just include in instructor?
  Render() = default;
  virtual ~Render() = default;
  virtual void draw(const Scene &scene) = 0;
  virtual std::shared_ptr<Mesh> createMesh(const std::string &path) = 0;
  virtual std::shared_ptr<Texture> createTexture(const std::string &path) = 0;
  virtual std::shared_ptr<Pipeline>
  createPipeline(const std::vector<std::string> &shaderPaths) = 0;
};
