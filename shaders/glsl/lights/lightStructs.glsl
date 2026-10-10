struct Surface {
  vec3 normal;
  vec3 viewDir;
  vec3 albedo;
  vec3 F0;
  float metallic;
  float roughness;
};

struct LightSample {
  vec3 L;
  vec3 radiance;
};