#include "material.h"
#include "../gfx/render.h"

Material::Material(
    Render &renderer, const std::vector<std::string> &shaderPaths,
    const std::map<std::string, std::shared_ptr<Texture>> &texturesInput,
    const std::map<std::string, UniformValue> &uniformsInput)
    : pipeline(renderer.createPipeline(shaderPaths)), uniforms(uniformsInput) {
  for (const auto &[name, texPtr] : texturesInput) {
    textures[name] = texPtr;
  }
}
