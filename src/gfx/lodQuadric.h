#pragma once
#include <cstdint>

struct lodQuadric {
  float    q[10];
  float    avg[3];
  uint32_t count;
};
static_assert(sizeof(lodQuadric) == 56);