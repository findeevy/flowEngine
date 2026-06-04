#pragma once

#include <glm/vec3.hpp>
#include <qlm/quaternion.hpp>

class Camera {
public:
  // settings
  float fov = 90.0f;
  float nearClip = 0.001f;
  float farClip = 1000.0f;

  // location
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};
