#include "/pbr/distribution.glsl"
#include "/pbr/fresnel.glsl"
#include "/pbr/geometry.glsl"

vec3 cookTorranceSpecular(vec3 N, vec3 V, vec3 L, vec3 H, vec3 F0, float rough,
                          out vec3 outF) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float VdotH = max(dot(V, H), 0.0);

  float D = distributionGGX(N, H, rough);
  float G = geometrySmith(N, V, L, rough);
  vec3 F = fresnelSchlick(VdotH, F0);
  outF = F;

  vec3 numerator = D * G * F;
  float denominator = 4.0 * NdotV * NdotL + 1e-4;

  return numerator / denominator;
}