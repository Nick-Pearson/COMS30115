#ifndef MATERIAL_H
#define MATERIAL_H

#include "material.h"

#include <glm/glm.hpp>

class Material
{
public:

  Material(const glm::vec3& Albedo, const glm::vec3& Specular = glm::vec3(0.0f, 0.0f, 0.0f), const float Mirror = 0.0f, const float Emissive = 0.0f, const float IOR = 1.0f) :
    albedo(Albedo), specular(Specular), mirror(Mirror), emissive(Emissive), ior(IOR)
  {}

	virtual glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) = 0;

  //returns a reflection ray according to importance sampling for this material
  glm::vec3 CalculateReflectedRay(const glm::vec3& in_ray, const glm::vec3& normal) const;


  glm::vec3 albedo;
  glm::vec3 specular;
  float mirror;
  float emissive;
  float ior;
};

#endif
