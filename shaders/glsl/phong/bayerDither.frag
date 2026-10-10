// borked

#version 430 core
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
in mat3 TBN;
uniform vec3 viewPosition;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D emissionMap;
uniform sampler2D normalMap;
uniform bool hasDiffuse;
uniform bool hasSpecular;
uniform bool hasEmission;
uniform bool hasNormal;
uniform vec3 diffuseTint;
uniform vec3 specularTint;
uniform vec3 emissionTint;

struct PointLight {
  vec4 color;
  vec3 position;
  float _pad;
};

layout(std430, binding = 0) buffer LightBlock {
  int lightCount;
  int _pad[3];
  PointLight lights[];
};

out vec4 fragColor;

void main() {

  vec4 diffuseTex = hasDiffuse ? texture(diffuseMap, fragTexCoord) : vec4(1.0);
  vec4 specularTex =
      hasSpecular ? texture(specularMap, fragTexCoord) : vec4(1.0);
  vec4 emission = hasEmission ? texture(emissionMap, fragTexCoord) : vec4(0.0);

  vec3 normal;
  if (hasNormal) {
    vec3 sampledNormal = texture(normalMap, fragTexCoord).rgb * 2.0 - 1.0;
    normal = normalize(TBN * sampledNormal);
  } else {
    normal = normalize(fragNormal);
  }

  vec3 viewDirection = normalize(viewPosition - fragPos);

  vec3 accDiffuse = vec3(0.0);
  vec3 accSpecular = vec3(0.0);

  for (int i = 0; i < lightCount; i++) {
    vec3 lightDir = normalize(lights[i].position - fragPos);
    vec3 halfVector = normalize(lightDir + viewDirection);
    float lightDistance = length(lights[i].position - fragPos);
    float radius = lights[i].color.a;
    float attenuation = clamp(
        1.0 - (lightDistance * lightDistance) / (radius * radius), 0.0, 1.0);
    attenuation *= attenuation;

    float lambert = max(dot(normal, lightDir), 0.0);
    vec3 diffuseContrib = lambert * lights[i].color.rgb;

    float shininess = 256.0;
    float specScalar = pow(max(dot(normal, halfVector), 0.0), shininess);
    vec3 specularContrib = specScalar * lights[i].color.rgb;

    accDiffuse += diffuseContrib * attenuation;
    accSpecular += specularContrib * attenuation;
  }

  vec3 litColor = (diffuseTex.rgb * diffuseTint * accDiffuse) +
                  (specularTex.rgb * specularTint * accSpecular) + emission.rgb;

  int bayerLocation = (int(gl_FragCoord.x) % 4) + 4 * (int(gl_FragCoord.y) % 4);
  vec3 final =
      (((litColor.r + litColor.g + litColor.b) / 3) > bayer4[bayerLocation])
          ? vec3(1.0)
          : vec3(0.0);

  fragColor = vec4(final, diffuseTex.a);
}
