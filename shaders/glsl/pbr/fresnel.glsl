vec3 fresnelSchlick(float VdotH, vec3 F0) {
  return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}