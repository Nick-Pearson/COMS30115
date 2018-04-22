#include "sphere.h"

#include "../amath.h"

#include <glm/gtx/norm.hpp>

bool Sphere::Intersect(const glm::vec3& start, const glm::vec3& dir, float& outDistance, glm::vec3& outNormal) const
{
  const glm::vec3 L = center - start;
  float tca = glm::dot(L, dir);

  if(tca < 0.0f)
    return false;

  float d2 = glm::dot(L,L) - AMath::square(tca);
  float radius2 = AMath::square(radius);

  if (d2 > radius2)
    return false;

  float thc = sqrt(radius2 - d2);

  float t0 = tca - thc;
  float t1 = tca + thc;

  if (t0 > t1) std::swap(t0, t1);

  if (t0 < 0.0f)
  {
    t0 = t1;
    if (t0 < 0.0f)
      return false;
  }

  outDistance = t0;
  outNormal = glm::normalize((start + (outDistance * dir)) - center);
  return true;
}

std::shared_ptr<Mesh> Sphere::GenerateMesh() const
{
  // TODO:
  return nullptr;
}
