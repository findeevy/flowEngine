#include "loader.h"

static bool hasExtension(const std::string &path, const std::string &ext) {
  return path.size() >= ext.size() &&
         path.compare(path.size() - ext.size(), ext.size(), ext) == 0;
}

static glm::vec3 vec3At(const nlohmann::json &j) {
  return glm::vec3(j.at(0).get<float>(), j.at(1).get<float>(),
                   j.at(2).get<float>());
}

static std::shared_ptr<Material> loadMaterial(const std::string &filePath,
                                              Render &renderer) {
  try {
    if (!hasExtension(filePath, ".grss")) {
      LOG_EVENT("Not a .grss file.");
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
      LOG_EVENT("Could not open .grss file.");
    }

    nlohmann::json matFile;
    file >> matFile;

    std::vector<std::string> shaderPaths =
        matFile.at("shaders").get<std::vector<std::string>>();

    std::map<std::string, std::shared_ptr<Texture>> textures;
    for (const auto &[texName, texPath] : matFile.at("textures").items()) {
      textures[texName] = renderer.createTexture(texPath.get<std::string>());
    }

    std::map<std::string, UniformValue> uniforms;
    for (const auto &[uniName, uniData] : matFile.at("uniforms").items()) {
      std::string type = uniData.at("type").get<std::string>();
      const auto &value = uniData.at("value");

      if (type == "int") {
        uniforms[uniName] = value.get<int>();
      } else if (type == "float") {
        uniforms[uniName] = value.get<float>();
      } else if (type == "bool") {
        uniforms[uniName] = value.get<bool>();
      } else {
        std::vector<float> v = value.get<std::vector<float>>();

        if (type == "vec2") {
          uniforms[uniName] = glm::make_vec2(v.data());
        } else if (type == "vec3") {
          uniforms[uniName] = glm::make_vec3(v.data());
        } else if (type == "vec4") {
          uniforms[uniName] = glm::make_vec4(v.data());
        } else if (type == "mat2") {
          uniforms[uniName] = glm::make_mat2(v.data());
        } else if (type == "mat3") {
          uniforms[uniName] = glm::make_mat3(v.data());
        } else if (type == "mat4") {
          uniforms[uniName] = glm::make_mat4(v.data());
        } else {
          LOG_EVENT("Unknown uniform type: " + type);
        }
      }
    }

    return std::make_shared<Material>(renderer, shaderPaths, textures,
                                      uniforms);
  } catch (const std::exception &e) {
    throw std::runtime_error("material " + filePath + ": " + e.what());
  }
}

static GameObject loadGameObject(
    const std::string &name, const std::string &filePath, Render &renderer,
    const std::map<std::string, std::shared_ptr<Material>> &materials,
    std::map<std::string, std::shared_ptr<Mesh>> &meshes) {
  try {
    if (!hasExtension(filePath, ".tree")) {
      throw std::runtime_error("Not a .tree file.");
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open .tree file.");
    }

    nlohmann::json objFile;
    file >> objFile;

    std::string meshPath = objFile.at("mesh").get<std::string>();
    std::string matName = objFile.at("material").get<std::string>();

    auto mat = materials.find(matName);
    if (mat == materials.end()) {
      throw std::runtime_error("Unknown material: " + matName);
    }

    auto mesh = meshes.find(meshPath);
    if (mesh == meshes.end()) {
      mesh = meshes.emplace(meshPath, renderer.createMesh(meshPath)).first;
    }

    return GameObject(name, mesh->second, mat->second, glm::vec3(0.0f),
                      glm::quat(glm::vec3(0.0f)));
  } catch (const std::exception &e) {
    throw std::runtime_error("object " + filePath + ": " + e.what());
  }
}

Scene loadScene(const std::string &filePath, Render &renderer) {
  if (!hasExtension(filePath, ".wrld")) {
    LOG_EVENT("Not a .wrld file: " + filePath);
  }

  std::ifstream file(filePath);
  if (!file.is_open()) {
    LOG_EVENT("Could not open .wrld file: " + filePath);
  }

  nlohmann::json sceneFile;
  file >> sceneFile;

  Scene scene;

  const auto &camData = sceneFile.at("camera");
  scene.camera.fov = camData.at("fov");
  scene.camera.farClip = camData.at("farClip");
  scene.camera.nearClip = camData.at("nearClip");
  scene.camera.position = vec3At(camData.at("position"));
  scene.camera.rotation = glm::quat(
      camData.at("rotation").at(0).get<float>(),
      camData.at("rotation").at(1).get<float>(),
      camData.at("rotation").at(2).get<float>(),
      camData.at("rotation").at(3).get<float>());

  std::map<std::string, std::shared_ptr<Material>> materials;
  for (const auto &[matName, matPath] : sceneFile.at("materials").items()) {
    materials[matName] = loadMaterial(matPath.get<std::string>(), renderer);
  }

  std::map<std::string, std::shared_ptr<Mesh>> meshes;
  for (const auto &[objName, obj] : sceneFile.at("gameObjects").items()) {
    scene.gameObjects.emplace_back(
    loadGameObject(objName, obj.at("path").get<std::string>(), renderer,
                   materials, meshes));

    auto &gameObject = scene.gameObjects.back();
    gameObject.position = vec3At(obj.at("position"));
    gameObject.rotation =
        glm::quat(glm::radians(vec3At(obj.at("rotation"))));
  }

  for (const auto &lightData : sceneFile.at("pointLights")) {
    PointLight light;
    light.color = glm::vec4(vec3At(lightData.at("color")),
                            lightData.at("intensity").get<float>());
    light.position = vec3At(lightData.at("position"));
    scene.pointLights.emplace_back(light);
  }

  for (const auto &lightData : sceneFile.at("directionalLights")) {
    DirectionalLight light;
    light.color = glm::vec4(vec3At(lightData.at("color")),
                            lightData.at("intensity").get<float>());
    light.direction = glm::normalize(vec3At(lightData.at("direction")));
    scene.directionalLights.emplace_back(light);
  }

  for (const auto &lightData : sceneFile.at("spotLights")) {
    SpotLight light;
    light.color = glm::vec4(vec3At(lightData.at("color")),
                            lightData.at("intensity").get<float>());
    light.position = vec3At(lightData.at("position"));
    light.direction = glm::normalize(vec3At(lightData.at("direction")));
    light.innerCutoff =
        glm::cos(glm::radians(lightData.at("innerAngle").get<float>()));
    light.outerCutoff =
        glm::cos(glm::radians(lightData.at("outerAngle").get<float>()));
    light.constant = lightData.at("constant");
    light.linear = lightData.at("linear");
    light.quadratic = lightData.at("quadratic");
    scene.spotLights.emplace_back(light);
  }

  LOG_EVENT("Loaded file: " + filePath);

  return scene;
}