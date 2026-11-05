#pragma once
#include <cstdint>

struct lodQuadric {
  float    q[10]{};
  float    avg[3]{};
  uint32_t count = 0;
};
static_assert(sizeof(lodQuadric) == 56);