#include "/constants/pi.glsl"
#include "/pbr/cookTorrance.glsl"

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

vec3 shadeLight(Surface s, LightSample l) {
  float NdotL = max(dot(s.normal, l.L), 0.0);
  if (NdotL <= 0.0)
    return vec3(0.0);

  vec3 H = normalize(l.L + s.viewDir);
  vec3 F;
  vec3 spec =
      cookTorranceSpecular(s.normal, s.viewDir, l.L, H, s.F0, s.roughness, F);
  vec3 kd = (vec3(1.0) - F) * (1.0 - s.metallic);
  vec3 diffuse = kd * s.albedo / PI;

  return (diffuse + spec) * l.radiance * NdotL;
}