#include "/pbr/pbr.glsl"

struct DirectionalLight {
  vec4 color;
  vec3 direction;
  float dirPad;
};

layout(std430, binding = 1) buffer DirectionalLightBlock {
  int directionalLightCount;
  int _dpad[3];
  DirectionalLight dirLights[];
};

LightSample directionalLightSample(int i) {
  return LightSample(normalize(-dirLights[i].direction),
                     dirLights[i].color.rgb);
}