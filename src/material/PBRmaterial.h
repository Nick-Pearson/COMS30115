#ifndef PBRMATERIAL_H
#define PBRMATERIAL_H

#include "material.h"

#include <glm/glm.hpp>

class PBRMaterial : public Material
{
public:

  glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) override;

private:	

  float Distribution(const glm::vec3& h, const glm::vec3& normal);

  float Fresnel(const glm::vec3& view, const glm::vec3& h, const glm::vec3& normal);

  float Geometry(const glm::vec3& light, const glm::vec3& view, const glm::vec3& normal);
  float Geometry_Internal(const glm::vec3& vector, const glm::vec3& normal);
};

#endif
