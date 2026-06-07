#pragma once

#include <stb/stb_image.h>
#include <string>

class Texture {
public:
  int width;
  int height;
  int channels;
  unsigned char *data;

  Texture(const std::string &filePath);

  virtual void bind(int slot) = 0;
  virtual ~Texture();
};
