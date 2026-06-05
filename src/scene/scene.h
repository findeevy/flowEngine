#include "camera.h"
#include "gameObject.h"
#include "pointLight.h"

#include <vector>

class Scene {
public:
  Scene(Camera &_camera, std::vector<GameObject> &_gameObjects,
        std::vector<PointLight> &_pointLights);
  Camera camera;
  std::vector<GameObject> gameObjects;
  std::vector<PointLight> pointLights;
};
