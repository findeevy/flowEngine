#pragma once

#include "../pipeline.h"
#include "oglShader.h"
#include <glad/gl.h>

class OGLPipeline : public Pipeline {
public:
  // look into other api's equiv, set those to be part of the Pipeline virt
  // func/constructor
  OGLShader shader;

  bool depthTest = true;
  bool depthWrite = true;

  GLenum depthFunc = GL_LESS;

  bool blendEnable = false;
  GLenum blendSrc = GL_SRC_ALPHA;
  GLenum blendDest = GL_ONE_MINUS_SRC_ALPHA;

  bool cullFace = true;
  GLenum cullMode = GL_BACK;

  OGLPipeline(const std::string &vertexPath, const std::string &fragmentPath);

  void bind();
};
