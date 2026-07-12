#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

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

const float PI = 3.14159265359;

struct PointLight {
  vec4 color;
  vec3 position;
  float pointPad;
};

struct DirectionalLight {
  vec4 color;
  vec3 direction;
  float dirPad;
};

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

layout(std430, binding = 0) buffer PointLightBlock {
  int pointLightCount;
  int _ppad[3];
  PointLight pointLights[];
};

layout(std430, binding = 1) buffer DirectionalLightBlock {
  int directionalLightCount;
  int _dpad[3];
  DirectionalLight dirLights[];
};

layout(std430, binding = 2) buffer SpotLightBlock {
  int spotLightCount;
  int _spad[3];
  SpotLight spotLights[];
};

out vec4 fragColor;

float distributionGGX(vec3 N, vec3 H, float rough) {
  float a = rough * rough;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return a2 / max(denom, 1e-6);
}

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

vec3 fresnelSchlick(float VdotH, vec3 F0) {
  return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 cookTorranceSpecular(vec3 N, vec3 V, vec3 L, vec3 H, vec3 F0, float rough,
                          out vec3 outF) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float VdotH = max(dot(V, H), 0.0);

  float D = distributionGGX(N, H, rough);
  float G = geometrySmith(N, V, L, rough);
  vec3 F = fresnelSchlick(VdotH, F0);
  outF = F;

  vec3 numerator = D * G * F;
  float denominator = 4.0 * NdotV * NdotL + 1e-4;

  return numerator / denominator;
}

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

  vec3 normal =
      hasNormal ? normalize(texture(normalMap, fragTexCoord).rgb * 2.0 - 1.0)
                : normalize(fragNormal);

  vec3 viewDirection = normalize(viewPosition - fragPos);

  vec3 albedo = albedoTex.rgb * albedoTint;

  vec3 F0 = mix(vec3(0.04), albedo, metallic);

  vec3 Lo = vec3(0.0);

  for (int i = 0; i < pointLightCount; i++) {
    vec3 lightDir = normalize(pointLights[i].position - fragPos);
    vec3 halfVector = normalize(lightDir + viewDirection);

    float lightDistance = length(pointLights[i].position - fragPos);
    float radius = pointLights[i].color.a;

    float attenuation = clamp(
        1.0 - (lightDistance * lightDistance) / (radius * radius), 0.0, 1.0);
    attenuation *= attenuation;

    float NdotL = max(dot(normal, lightDir), 0.0);
    if (NdotL > 0.0) {
      vec3 F;
      vec3 spec = cookTorranceSpecular(normal, viewDirection, lightDir,
                                       halfVector, F0, roughness, F);
      vec3 kd = (vec3(1.0) - F) * (1.0 - metallic);
      vec3 diffuse = kd * albedo / PI;

      vec3 radiance = pointLights[i].color.rgb * attenuation;
      Lo += (diffuse + spec) * radiance * NdotL;
    }
  }

  for (int i = 0; i < directionalLightCount; i++) {
    vec3 lightDir = normalize(-dirLights[i].direction);
    vec3 halfVector = normalize(lightDir + viewDirection);

    float NdotL = max(dot(normal, lightDir), 0.0);
    if (NdotL > 0.0) {
      vec3 F;
      vec3 spec = cookTorranceSpecular(normal, viewDirection, lightDir,
                                       halfVector, F0, roughness, F);
      vec3 kd = (vec3(1.0) - F) * (1.0 - metallic);
      vec3 diffuse = kd * albedo / PI;

      vec3 radiance = dirLights[i].color.rgb;
      Lo += (diffuse + spec) * radiance * NdotL;
    }
  }

  for (int i = 0; i < spotLightCount; i++) {
    vec3 lightDir = normalize(spotLights[i].position - fragPos);
    vec3 halfVector = normalize(lightDir + viewDirection);

    float lightDistance = length(spotLights[i].position - fragPos);

    float theta = dot(lightDir, normalize(-spotLights[i].direction));
    float epsilon = (spotLights[i].innerCutOff - spotLights[i].outerCutOff);
    float fallout =
        clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

    float attenuation =
        1.0 / (spotLights[i].constant + spotLights[i].linear * lightDistance +
               spotLights[i].quadratic * (lightDistance * lightDistance));

    float NdotL = max(dot(normal, lightDir), 0.0);
    if (NdotL > 0.0) {
      vec3 F;
      vec3 spec = cookTorranceSpecular(normal, viewDirection, lightDir,
                                       halfVector, F0, roughness, F);
      vec3 kd = (vec3(1.0) - F) * (1.0 - metallic);
      vec3 diffuse = kd * albedo / PI;

      vec3 radiance = spotLights[i].color.rgb * fallout * attenuation;
      Lo += (diffuse + spec) * radiance * NdotL;
    }
  }

  vec3 ambient = ambientLight * albedo * ao;

  vec3 litColor = ambient + Lo + (emissive.rgb * emissiveTint);

  fragColor = vec4(litColor, albedoTex.a);
}
