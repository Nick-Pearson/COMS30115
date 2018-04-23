#include "material.h"

#include <random>

#include <glm/gtx/transform.hpp>

glm::vec3 Material::CalculateReflectedRay(const glm::vec3& in_ray, const glm::vec3& normal) const
{
  //randomly select between a direct mirror and a diffuse ray
  bool useReflectionRay = ((float)rand() / (float)RAND_MAX) <= mirror;

  if(useReflectionRay)
  {
    return glm::normalize(in_ray - (2.0f * glm::dot(in_ray, normal) * normal));
  }

  // add a random direction of indirect light
  const float theta1 = (float)rand() * 3.14f / (float)RAND_MAX;
  const float theta2 = (float)rand() * 3.14f / (float)RAND_MAX;
  const float theta3 = (float)rand() * 3.14f / (float)RAND_MAX;

  glm::mat4 rotationMatrix;
  rotationMatrix = glm::rotate(rotationMatrix, theta1, glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, theta2, glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, theta3, glm::vec3(0.0f, 0.0f, 1.0f));

  return glm::vec3(glm::vec4(normal, 1.0f) * rotationMatrix);
}
