#pragma once

#include "../gfx/render.h"
#include "../scene/scene.h"
#include "../utils/logger.h"

#include <string>

#include <nlohmann/json.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>

class Render;

Scene loadScene(const std::string &filePath, Render &renderer);