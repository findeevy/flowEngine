#include "oglRender.h"
#include "../../utils/logger.h"
#include "oglMesh.h"
#include "oglPipeline.h"
#include "oglTexture.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

OGLRender::OGLRender() {
  if (!gladLoadGL(glfwGetProcAddress)) {
    LOG_EVENT("Failed to initialize GLAD");
    exit(0);
  }
  glEnable(GL_DEPTH_TEST);
  glGenBuffers(1, &lightSSBO);
}

std::shared_ptr<Texture> OGLRender::createTexture(const std::string &path) {
  return std::make_shared<OGLTexture>(path);
}

std::shared_ptr<Mesh> OGLRender::createMesh(const std::string &path) {
  return std::make_shared<OGLMesh>(path);
}

std::shared_ptr<Pipeline>
OGLRender::createPipeline(const std::vector<std::string> &shaderPaths) {
  return std::make_shared<OGLPipeline>(shaderPaths);
}

void OGLRender::draw(const Scene &_scene) {
  glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::vector<OGLPointLight> oglPointLights;
  for (const auto &light : _scene.pointLights) {
    oglPointLights.emplace_back(
        OGLPointLight{light.color, light.position, 0.0f});
  }

  struct SSBOHeader {
    int lightCount;
    int _pad[3];
  } header;
  header.lightCount = (int)oglPointLights.size();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(SSBOHeader) +
                   oglPointLights.size() * sizeof(OGLPointLight),
               nullptr, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SSBOHeader), &header);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(SSBOHeader),
                  oglPointLights.size() * sizeof(OGLPointLight),
                  oglPointLights.data());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightSSBO);

  glm::mat4 view = glm::mat4_cast(glm::inverse(_scene.camera.rotation)) *
                   glm::translate(glm::mat4(1.0f), -_scene.camera.position);

  int frameWidth;
  int frameHeight;
  glfwGetFramebufferSize(glfwGetCurrentContext(), &frameWidth, &frameHeight);
  glm::mat4 proj = glm::perspective(
      glm::radians(_scene.camera.fov), (float)frameWidth / (float)frameHeight,
      _scene.camera.nearClip, _scene.camera.farClip);

  for (const auto &object : _scene.gameObjects) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), object.position) *
                      glm::mat4_cast(object.rotation);
    auto pipeline =
        std::static_pointer_cast<OGLPipeline>(object.material->pipeline);
    pipeline->bind();

    pipeline->shader.setMat4("model", model);
    pipeline->shader.setMat4("view", view);
    pipeline->shader.setMat4("proj", proj);
    pipeline->shader.setVec3("viewPosition", _scene.camera.position);

    auto &mat = *object.material;

    for (auto const &[key, val] : mat.uniforms) {
      std::visit(
          [&](auto const &value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, int>)
              pipeline->shader.setInt(key, value);
            else if constexpr (std::is_same_v<T, float>)
              pipeline->shader.setFloat(key, value);
            else if constexpr (std::is_same_v<T, bool>)
              pipeline->shader.setBool(key, value);
            else if constexpr (std::is_same_v<T, glm::vec2>)
              pipeline->shader.setVec2(key, value);
            else if constexpr (std::is_same_v<T, glm::vec3>)
              pipeline->shader.setVec3(key, value);
            else if constexpr (std::is_same_v<T, glm::vec4>)
              pipeline->shader.setVec4(key, value);
            else if constexpr (std::is_same_v<T, glm::mat2>)
              pipeline->shader.setMat2(key, value);
            else if constexpr (std::is_same_v<T, glm::mat3>)
              pipeline->shader.setMat3(key, value);
            else if constexpr (std::is_same_v<T, glm::mat4>)
              pipeline->shader.setMat4(key, value);
          },
          val);
    }

    int textureBind = 0;
    for (auto const &[key, val] : mat.textures) {
      val->bind(textureBind);
      pipeline->shader.setInt(key, textureBind);
      textureBind++;
    }

    object.mesh->draw();
  }
}
