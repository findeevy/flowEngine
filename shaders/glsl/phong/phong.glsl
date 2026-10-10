vec3 shadeLight(Surface s, LightSample l) {
  float lambert = max(dot(s.normal, l.L), 0.0);
  if (lambert <= 0.0)
    return vec3(0.0);

  vec3 diffuse = s.albedo * lambert;
  vec3 reflect = 2*(dot(l.L, s.normal))*s.normal-l.L;
  float shininess = 1/((s.roughness*s.roughness));
  vec3 specular = vec3(pow(max(0.0f, dot(reflect, s.viewDir)),shininess));

  return (diffuse + specular) * l.radiance;
}