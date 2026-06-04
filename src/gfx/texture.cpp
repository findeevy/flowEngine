#include "texture.h"
#include "../utils/logger.h"
#include "std_image.h"

#include <stdexcept>

Texture::Texture(const std::string &filePath) {
  data = stbi_load(FileSystem::getPath(filePath).c_str(), &width, &height,
                   &channels, 0);

  if (!data || width <= 0 || height <= 0 || channels <= 0) {
    throw std::runtime_error("Failed to load texture: " + filePath);
  }

  LOG_EVENT(filePath);
}

Texture::~Texture() {
  stbi_image_free(data);
  data = nullptr;
}
