#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H

#include "material.h"

#include <glm/glm.hpp>

class PhongMaterial : public Material
{
public:
  PhongMaterial(const glm::vec3& Diffuse, const glm::vec3& Specular = glm::vec3(0.0f, 0.0f, 0.0f), float SpecularExponent = 10.0f) :
    diffuse(Diffuse), specular(Specular), specularExponent(SpecularExponent)
  {}

	glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) override;

  glm::vec3 diffuse;
  glm::vec3 specular;
  float specularExponent;
};

#endif // !PHONGMATERIAL_H
