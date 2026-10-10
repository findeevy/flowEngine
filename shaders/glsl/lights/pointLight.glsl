#include "/lights/lightStructs.glsl"

struct PointLight {
  vec4 color;
  vec3 position;
  float pointPad;
};

layout(std430, binding = 0) buffer PointLightBlock {
  int pointLightCount;
  int _ppad[3];
  PointLight pointLights[];
};

LightSample pointLightSample(int i, vec3 fragPos) {
  vec3 toLight = pointLights[i].position - fragPos;
  float dist = length(toLight);
  float radius = pointLights[i].color.a;

  float attenuation = clamp(1.0 - (dist * dist) / (radius * radius), 0.0, 1.0);
  attenuation *= attenuation;

  return LightSample(toLight / dist, pointLights[i].color.rgb * attenuation);
}