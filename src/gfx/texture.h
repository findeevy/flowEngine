#pragma once

#include <string>

class Texture {
public:
  int width;
  int height;
  int channels;
  unsigned char *data;

  Texture(const std::string &filePath);

  ~Texture();
};
