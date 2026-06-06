#include "material.h"

Material::Material(const Pipeline &_pipeline, const std::string &diffusePath,
                   const std::string &specularPath,
                   const std::string &emissionPath,
                   const glm::vec3 &_diffuseTint,
                   const glm::vec3 &_specularTint,
                   const glm::vec3 &_emissionTint)
    : pipeline(std::make_shared<Pipeline>(_pipeline)),
      diffuseMap(std::make_shared<Texture>(diffusePath)),
      emissionMap(std::make_shared<Texture>(emissionPath))
          specularMap(std::make_shared<Texture>(specularPath)),
      diffuseTint(_diffuseTint), specularTint(_specularTint),
      emissionTint(_emissionTint),
      normalMap(std::make_shared<Texture>(normalPath)) {}
