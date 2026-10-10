float geometrySchlickGGX(float NdotX, float k) {
  return NdotX / (NdotX * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float rough) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float r = rough + 1.0;
  float k = (r * r) / 8.0;

  return geometrySchlickGGX(NdotV, k) * geometrySchlickGGX(NdotL, k);
}