#version 430 core

#include "/lights/directionalLight.glsl"
#include "/lights/pointLight.glsl"
#include "/lights/spotLight.glsl"

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
in mat3 TBN;

uniform vec3 viewPosition;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;

uniform bool hasAlbedo;
uniform bool hasNormal;
uniform bool hasMetallic;
uniform bool hasRoughness;
uniform bool hasAO;
uniform bool hasEmissive;

uniform vec3 albedoTint;
uniform vec3 emissiveTint;

uniform float metallicFactor;
uniform float roughnessFactor;

uniform vec3 ambientLight;

out vec4 fragColor;

void main() {
  vec4 albedoTex = hasAlbedo ? texture(albedoMap, fragTexCoord) : vec4(1.0);
  vec4 emissive = hasEmissive ? texture(emissiveMap, fragTexCoord) : vec4(0.0);
  float ao = hasAO ? texture(aoMap, fragTexCoord).r : 1.0;

  float metallic = hasMetallic
                       ? texture(metallicMap, fragTexCoord).r * metallicFactor
                       : metallicFactor;
  float roughness =
      hasRoughness ? texture(roughnessMap, fragTexCoord).r * roughnessFactor
                   : roughnessFactor;
  roughness = clamp(roughness, 0.045, 1.0);
  metallic = clamp(metallic, 0.0, 1.0);

  vec3 normal;
  if (hasNormal) {
    vec3 sampledNormal = texture(normalMap, fragTexCoord).rgb * 2.0 - 1.0;
    normal = normalize(TBN * sampledNormal);
  } else {
    normal = normalize(fragNormal);
  }

  vec3 viewDirection = normalize(viewPosition - fragPos);

  vec3 albedo = albedoTex.rgb * albedoTint;

  vec3 F0 = mix(vec3(0.04), albedo, metallic);

  Surface surf =
      Surface(normal, viewDirection, albedo, F0, metallic, roughness);

  vec3 Lo = vec3(0.0);

  for (int i = 0; i < pointLightCount; i++)
    Lo += shadeLight(surf, pointLightSample(i, fragPos));

  for (int i = 0; i < directionalLightCount; i++)
    Lo += shadeLight(surf, directionalLightSample(i));

  for (int i = 0; i < spotLightCount; i++)
    Lo += shadeLight(surf, spotLightSample(i, fragPos));

  vec3 ambient = ambientLight * albedo * ao;

  vec3 litColor = ambient + Lo + (emissive.rgb * emissiveTint);

  fragColor = vec4(litColor, albedoTint.r * 0.0 + albedoTex.a);
}