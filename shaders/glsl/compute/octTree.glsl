// Probabilistic octree (DeCoro & Tatarchuk): one fixed-size hash table per
// level, no pointers, no dynamic allocation. A node is addressed by its dense
// index k inside its level (paper: "compute the array index k as if the
// cluster was densely stored"), and the table stores k as the key so READ can
// detect hash collisions.
//
// Level l has resolution (root * 2^l) per axis. Slot index is global
// (level offset + slot in level), and is used directly as the cluster id.

const uint NONE = 0xFFFFFFFFu;
const uint KEY_EMPTY = 0xFFFFFFFFu;
const uint MAX_PROBE = 64u;

layout(std430, binding = 22) buffer Keys { uint keys[]; };

uniform uint uLevels; // lmax + 1
uniform uint uLevelOffset[16];
uniform uint uLevelMask[16];
uniform uint uLevelRes[16];
uniform uint
    uLevelDirect[16];    // table has a slot per cell: slot = k, no hashing
uniform vec3 uCellSize0; // root cell size

uint hashU(uint x) {
  x ^= x >> 16;
  x *= 0x7feb352du;
  x ^= x >> 15;
  x *= 0x846ca68bu;
  x ^= x >> 16;
  return x;
}

// dense index of the level-l cell containing pos (pos is relative to aabb min)
uint nodeKey(vec3 pos, uint l) {
  uint res = uLevelRes[l];
  vec3 cell = uCellSize0 / float(1u << l);
  ivec3 n = clamp(ivec3(floor(pos / cell)), ivec3(0), ivec3(int(res) - 1));
  return uint(n.x) + res * (uint(n.y) + res * uint(n.z));
}

// READ: global slot of node k at level l, or NONE if it isn't stored
uint findSlot(uint l, uint k) {
  uint base = uLevelOffset[l];
  if (uLevelDirect[l] != 0u)
    return keys[base + k] == k ? base + k : NONE;
  uint s = hashU(k) & uLevelMask[l];
  for (uint p = 0u; p < MAX_PROBE; p++) {
    uint cur = keys[base + s];
    if (cur == k)
      return base + s;
    if (cur == KEY_EMPTY)
      return NONE;
    s = (s + 1u) & uLevelMask[l];
  }
  return NONE;
}

// WRITE: claim (or find) the slot for node k at level l. NONE if the probe
// window is full (that sample is dropped; lookups fall back to the parent)
uint claimSlot(uint l, uint k) {
  uint base = uLevelOffset[l];
  if (uLevelDirect[l] != 0u) {
    keys[base + k] = k; // every writer stores the same value
    return base + k;
  }
  uint s = hashU(k) & uLevelMask[l];
  for (uint p = 0u; p < MAX_PROBE; p++) {
    uint prev = atomicCompSwap(keys[base + s], KEY_EMPTY, k);
    if (prev == KEY_EMPTY || prev == k)
      return base + s;
    s = (s + 1u) & uLevelMask[l];
  }
  return NONE;
}
