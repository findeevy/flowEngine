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

std::shared_ptr<Pipeline> OGLRender::createPipeline(const std::string &v,
                                                    const std::string &f) {
  return std::make_shared<OGLPipeline>(v, f);
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
  glm::mat4 proj =
      glm::perspective(glm::radians(_scene.camera.fov),
                       _scene.camera.width / _scene.camera.height,
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

    pipeline->shader.setVec3("diffuseTint", object.material->diffuseTint);
    pipeline->shader.setVec3("specularTint", object.material->specularTint);
    pipeline->shader.setVec3("emissionTint", object.material->emissionTint);

    auto &mat = *object.material;
    if (mat.diffuseMap.has_value()) {
      mat.diffuseMap.value()->bind(0);
      pipeline->shader.setInt("diffuseMap", 0);
      pipeline->shader.setBool("hasDiffuse", true);
    } else {
      pipeline->shader.setBool("hasDiffuse", false);
    }
    if (mat.specularMap.has_value()) {
      mat.specularMap.value()->bind(1);
      pipeline->shader.setInt("specularMap", 1);
      pipeline->shader.setBool("hasSpecular", true);
    } else {
      pipeline->shader.setBool("hasSpecular", false);
    }
    if (mat.emissionMap.has_value()) {
      mat.emissionMap.value()->bind(2);
      pipeline->shader.setInt("emissionMap", 2);
      pipeline->shader.setBool("hasEmission", true);
    } else {
      pipeline->shader.setBool("hasEmission", false);
    }
    if (mat.normalMap.has_value()) {
      mat.normalMap.value()->bind(3);
      pipeline->shader.setInt("normalMap", 3);
      pipeline->shader.setBool("hasNormal", true);
    } else {
      pipeline->shader.setBool("hasNormal", false);
    }

    object.mesh->draw();
  }
}
