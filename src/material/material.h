#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

class Material
{
public:
  // input vectors must be normalised
  static float CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal);

private:

  static float Distribution(const glm::vec3& h, const glm::vec3& normal);

  static float Fresnel(const glm::vec3& view, const glm::vec3& h, const glm::vec3& normal);

  static float Geometry(const glm::vec3& light, const glm::vec3& view, const glm::vec3& normal);
  static float Geometry_Internal(const glm::vec3& vector, const glm::vec3& normal);
};

#endif
