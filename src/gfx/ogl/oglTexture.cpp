#include "oglTexture.h"

void OGLTexture::bind(int slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, handle);
};

OGLTexture::OGLTexture(const std::string &filePath) : Texture(filePath) {
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

OGLTexture::~OGLTexture() { glDeleteTextures(1, &handle); }
