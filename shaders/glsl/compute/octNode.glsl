// one probabilistic-octree node: summed quadric + summed position + count (56
// B) needs floatAtomic.glsl included first
struct OctNode {
  ATOMIC_FLOAT_T q[10];
  ATOMIC_FLOAT_T p[3];
  uint count;
};
