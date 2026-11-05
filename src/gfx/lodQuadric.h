#pragma once
#include <cstdint>

struct lodQuadric {
  float q[10]{};
  float avg[3]{};
  float nor[3]{};
  float tex[2]{};
  uint32_t count = 0;
};

static_assert(sizeof(lodQuadric) == 76);