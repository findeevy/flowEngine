#include "scene.h"
#include "../utils/logger.h"
#include <glm/gtc/constants.hpp>

Scene::Scene(const Camera &_camera, const std::vector<GameObject> &_gameObjects,
             const std::vector<PointLight> &_pointLights,
             const std::vector<DirectionalLight> &_directionalLights,
             const std::vector<SpotLight> &_spotLights)
    : camera(_camera), gameObjects(_gameObjects), pointLights(_pointLights),
      directionalLights(_directionalLights), spotLights(_spotLights) {}

Scene::Scene(const std::string &filePath){
    try{
    std::ifstream file(filePath);
    nlohmann::json sceneFile;
    file >> sceneFile;
    camera.fov = sceneFile["camera"]["fov"];
    camera.farClip = sceneFile["camera"]["farClip"];
    camera.nearClip = sceneFile["camera"]["nearClip"];  
    camera.position.xyz(float x, float y, float z);
    camera.rotation.wxyz(float w, float x, float y, float z)
//add the rest lol


    LOG_EVENT("Loaded file: " << filePath);
    }

    catch (const std::exception &e) {
      LOG_EVENT("Failed to load level" << filePath << " due to error: " << e.what());
    }
}

void Scene::update(float dt) {
  // iterate through scene objects update/init code, delete, add.
}
