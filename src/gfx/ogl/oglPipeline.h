#pragma once

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

  cullFace = true;
  GLenum cullMode = CL_BACK;

  OGLPipeline(const std::string &vertexPath, const std::string &vertexPath);

  void bind();
}
