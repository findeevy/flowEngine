#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
  // settings
  float fov = 90.0f;
  float nearClip = 0.001f;
  float farClip = 1000.0f;

  float height = 1080.0f;
  float width = 1920.0f;

  // location
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};
