#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H

#include "material.h"

#include <glm/glm.hpp>

class PhongMaterial : public Material
{
public:
  PhongMaterial(const glm::vec3& Albedo, const glm::vec3& Specular = glm::vec3(0.0f, 0.0f, 0.0f), const float Mirror = 0.0f, const float Emissive = 0.0f, float SpecularExponent = 10.0f) :
    Material(Albedo, Specular, Mirror, Emissive), specularExponent(SpecularExponent)
  {}

	glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) override;

  float specularExponent;
};

#endif // !PHONGMATERIAL_H
