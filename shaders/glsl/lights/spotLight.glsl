#include "/lights/lightStructs.glsl"

struct SpotLight {
  vec4 color;
  vec3 direction;
  float innerCutOff;
  vec3 position;
  float outerCutOff;
  float constant;
  float linear;
  float quadratic;
};

layout(std430, binding = 2) buffer SpotLightBlock {
  int spotLightCount;
  int _spad[3];
  SpotLight spotLights[];
};

LightSample spotLightSample(int i, vec3 fragPos) {
  vec3 toLight = spotLights[i].position - fragPos;
  float dist = length(toLight);
  vec3 L = toLight / dist;

  float theta = dot(L, normalize(-spotLights[i].direction));
  float epsilon = spotLights[i].innerCutOff - spotLights[i].outerCutOff;
  float fallout =
      clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

  float attenuation =
      1.0 / (spotLights[i].constant + spotLights[i].linear * dist +
             spotLights[i].quadratic * dist * dist);

  return LightSample(L, spotLights[i].color.rgb * fallout * attenuation);
}