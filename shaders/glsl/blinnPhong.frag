#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

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
    float pointPad;
};

struct DirectionalLight {
    vec4 color;
    vec3 direction;
    float dirPad;
};

struct SpotLight {
  glm::vec4 color;
  glm::vec3 direction;
  float innerCutoff;
  float outerCutoff;
  float constant;
  float linear;
  float quadratic;
}

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
    int _dpad[3];
    SpotLight spotLights[];
};

out vec4 fragColor;

void main() {
    vec4 diffuseTex = hasDiffuse ? texture(diffuseMap, fragTexCoord) : vec4(1.0);
    vec4 specularTex = hasSpecular ? texture(specularMap, fragTexCoord) : vec4(1.0);
    vec4 emission = hasEmission ? texture(emissionMap, fragTexCoord) : vec4(0.0);

    vec3 normal = hasNormal
        ? normalize(texture(normalMap, fragTexCoord).rgb * 2.0 - 1.0)
        : normalize(fragNormal);

    vec3 viewDirection = normalize(viewPosition - fragPos);

    vec3 accDiffuse = vec3(0.0);
    vec3 accSpecular = vec3(0.0);

    for (int i = 0; i < pointLightCount; i++) {
        vec3 lightDir = normalize(pointLights[i].position - fragPos);
        vec3 halfVector = normalize(lightDir + viewDirection);

        float lightDistance = length(pointLights[i].position - fragPos);
        float radius = pointLights[i].color.a;

        float attenuation =
            clamp(1.0 - (lightDistance * lightDistance) / (radius * radius), 0.0, 1.0);
        attenuation *= attenuation;

        float lambert = max(dot(normal, lightDir), 0.0);
        vec3 diffuseContrib = lambert * pointLights[i].color.rgb;

        float specScalar = lambert > 0.0
            ? pow(max(dot(normal, halfVector), 0.0), 256.0)
            : 0.0;

        vec3 specularContrib = specScalar * pointLights[i].color.rgb;

        accDiffuse += diffuseContrib * attenuation;
        accSpecular += specularContrib * attenuation;
    }

    for (int i = 0; i < directionalLightCount; i++) {
        vec3 lightDir = normalize(-dirLights[i].direction);
        vec3 halfVector = normalize(lightDir + viewDirection);

        float lambert = max(dot(normal, lightDir), 0.0);
        vec3 diffuseContrib = lambert * dirLights[i].color.rgb;

        float specScalar = lambert > 0.0
            ? pow(max(dot(normal, halfVector), 0.0), 256.0)
            : 0.0;

        vec3 specularContrib = specScalar * dirLights[i].color.rgb;

        accDiffuse += diffuseContrib;
        accSpecular += specularContrib;
    }


    
    for (int i = 0; i < spotLightCount; i++) {
        vec3 lightDir = normalize(spotLights[i].position - fragPos);
        vec3 halfVector = normalize(lightDir + viewDirection);

        float lightDistance = length(spotLights[i].position - fragPos);
        float radius = spotLights[i].color.a;


        float theta = dot(lightDir, normalize(-spotLight[i].direction)); 
        float epsilon = (spotLight[i].innerCutOff - spotLight[i].outerCutOff);
        float fallout = clamp((theta - spotLight[i].outerCutOff) / epsilon, 0.0, 1.0);



        float lambert = max(dot(normal, lightDir), 0.0);
        vec3 diffuseContrib = lambert * spotLights[i].color.rgb * fallout;

        float specScalar = lambert > 0.0
            ? pow(max(dot(normal, halfVector), 0.0), 256.0)
            : 0.0;

        vec3 specularContrib = specScalar * spotLights[i].color.rgb * fallout;
        float lightDistance = length(pointLights[i].position - fragPos);
	float attenuation = 1.0 / (spotLight.constant + spotLight.linear * lightDistance + spotLight.quadratic * (lightDistance * lightDistance));

        accDiffuse += diffuseContrib * attenuation;
        accSpecular += specularContrib * attenuation;
    }

    vec3 litColor =
        (diffuseTex.rgb * diffuseTint * accDiffuse) +
        (specularTex.rgb * specularTint * accSpecular) +
        (emission.rgb * emissionTint);

    fragColor = vec4(litColor, diffuseTex.a);
}
