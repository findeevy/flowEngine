#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 viewPosition;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;
out mat3 TBN;

void main() {
  vec4 worldPos = model * vec4(aPosition, 1.0);
  fragPos = worldPos.xyz;

  mat3 normalMatrix = mat3(transpose(inverse(model)));
  fragNormal = normalMatrix * aNormal;
  fragTexCoord = aTexCoord;

  vec3 N = normalize(fragNormal);
  vec3 rawT = normalMatrix * aTangent;

  vec3 T = rawT - dot(rawT, N) * N;
  float tLen = length(T);
  if (tLen < 1e-6) {
    vec3 fallback = abs(N.x) < 0.9 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
    T = fallback - dot(fallback, N) * N;
  }
  T = normalize(T);

  vec3 B = normalize(cross(N, T));
  TBN = mat3(T, B, N);

  gl_Position = proj * view * worldPos;
}
