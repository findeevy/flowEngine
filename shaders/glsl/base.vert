#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 viewPosition;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
  vec4 worldPos = model * vec4(aPosition, 1.0);
  fragPos = worldPos.xyz;
  fragNormal = mat3(transpose(inverse(model))) * aNormal;
  fragTexCoord = aTexCoord;
  gl_Position = proj * view * worldPos;
}
