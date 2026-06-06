#pragma once

#include "../texture.h"
#include <glad/gl.h>
#include <string>

class OGLTexture : public Texture {
public:
  GLuint handle;
  OGLTexture(const std::string &filePath);
  void bind(int slot);
  ~OGLTexture();
}
