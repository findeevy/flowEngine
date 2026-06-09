#include "material.h"
#include "../gfx/render.h"

static std::optional<std::shared_ptr<Texture>>
loadIfProvided(Render &renderer, const std::string &path) {
  if (path.empty())
    return std::nullopt;
  return renderer.createTexture(path);
}

Material::Material(Render &renderer,
                   const std::vector<std::string> &shaderPaths,

                   const std::string &diffusePath,
                   const std::string &specularPath,
                   const std::string &emissionPath,
                   const std::string &normalPath, const glm::vec3 &_diffuseTint,
                   const glm::vec3 &_specularTint,
                   const glm::vec3 &_emissionTint)
    : pipeline(renderer.createPipeline(shaderPaths)),
      diffuseMap(loadIfProvided(renderer, diffusePath)),
      specularMap(loadIfProvided(renderer, specularPath)),
      emissionMap(loadIfProvided(renderer, emissionPath)),
      diffuseTint(_diffuseTint), specularTint(_specularTint),
      emissionTint(_emissionTint),
      normalMap(loadIfProvided(renderer, normalPath)) {}
