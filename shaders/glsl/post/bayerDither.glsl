#include "/constants/bayer4.glsl"

vec3 bayerDither(vec3 color){
  int bayerLocation = (int(gl_FragCoord.x) % 4) + 4 * (int(gl_FragCoord.y) % 4);
  vec3 final =
      (((color.r + color.g + color.b) / 3) > bayer4[bayerLocation])
          ? vec3(1.0)
          : vec3(0.0);

  return vec3(final);
}
