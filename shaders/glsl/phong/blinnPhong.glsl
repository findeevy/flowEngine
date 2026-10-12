vec3 shadeLight(Surface s, LightSample l) {
  float lambert = max(dot(s.normal, l.L), 0.0);
  if (lambert <= 0.0)
    return vec3(0.0);

  vec3 H = normalize(l.L + s.viewDir);
  float shininess = 1 / ((s.roughness * s.roughness));
  float spec = pow(max(dot(s.normal, H), 0.0), shininess);

  vec3 diffuse = s.albedo * lambert;
  vec3 specular = vec3(spec);

  return (diffuse + specular) * l.radiance;
}