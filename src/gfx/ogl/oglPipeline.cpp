#include "oglPipeline.h"

OGLPipeline::OGLPipeline(const std::string &vertexPath,
                         const std::string &fragmentPath)
    : shader(vertexPath, fragmentPath) {};

void OGLPipeline::bind() {
  depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  glDepthMask(depthWrite);
  glDepthFunc(depthFunc);

  blendEnable ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
  glBlendFunc(blendSrc, blendDest);

  cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
  glCullFace(cullMode);

  shader.bind();
}
