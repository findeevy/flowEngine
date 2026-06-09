#pragma once

#include "../render.h"
#include "oglPipeline.h"
#include "oglPointLight.h"
#include "oglRender.h"

#include <glad/gl.h>

class OGLRender : public Render {
public:
  OGLRender();
  void draw(const Scene &_scene) override;
  std::shared_ptr<Texture> createTexture(const std::string &path) override;
  std::shared_ptr<Mesh> createMesh(const std::string &path) override;
  std::shared_ptr<Pipeline>
  createPipeline(const std::vector<std::string> &shaderPaths) override;

private:
  GLuint lightSSBO = 0;
};
