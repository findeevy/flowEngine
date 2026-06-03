#include "material.h"

Material::Material(
    const std::string& diffusePath,
    const std::string& specularPath,
    const glm::vec3& _diffuseTint,
    const glm::vec3& _specularTint)
: 
  diffuseMap(std::make_shared<Texture>(diffusePath)),
  specularMap(std::make_shared<Texture>(specularPath)),	
  diffuseTint(_diffuseTint),
  specularTint(_specularTint)
{}
