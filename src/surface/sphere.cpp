#include "sphere.h"

#include "../amath.h"

#include <glm/gtx/norm.hpp>

bool Sphere::Intersect(const glm::vec3& start, const glm::vec3& dir, float& outDistance, glm::vec3& outNormal, bool checkBackfaces /*= true*/) const
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
  glm::vec3 n0 = (start + (t0 * dir)) - center;
  float t1 = tca + thc;
  glm::vec3 n1 = (start + (t1 * dir)) - center;

  if (t0 > t1) std::swap(t0, t1);

  if (t0 < 0.0f ||
    (checkBackfaces && glm::dot(n0, dir) >= 0.0f))
  {
    t0 = t1;
    n0 = n1;

    if (t0 < 0.0f ||
      (checkBackfaces && glm::dot(n0, dir) >= 0.0f))
      return false;
  }

  outDistance = t0;
  outNormal = glm::normalize(n0);
  return true;
}

std::shared_ptr<Mesh> Sphere::GenerateMesh() const
{
  // TODO:
  return nullptr;
}
