// for verts in a mesh
struct LodQuadric {
  ATOMIC_FLOAT_T q[10];
  ATOMIC_FLOAT_T posSum[3];
  ATOMIC_FLOAT_T norSum[3];
  ATOMIC_FLOAT_T texSum[2];
  uint count;
};

struct QuadricF {
  float q[10];
};

QuadricF toQuadricF(mat4 m) {
  QuadricF quad;

  quad.q[0] = m[0][0];
  quad.q[1] = m[0][1];
  quad.q[2] = m[0][2];
  quad.q[3] = m[0][3];

  quad.q[4] = m[1][1];
  quad.q[5] = m[1][2];
  quad.q[6] = m[1][3];

  quad.q[7] = m[2][2];
  quad.q[8] = m[2][3];

  quad.q[9] = m[3][3];

  return quad;
}