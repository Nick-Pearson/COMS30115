#ifndef SPHERE_H
#define SPHERE_H

#include "implicitsurface.h"

class Sphere : public ImplicitSurface
{
public:

  // ImplicitSurface interface
  bool Intersect(const glm::vec3& start, const glm::vec3& dir, float& outDistance, glm::vec3& outNormal, bool checkBackfaces = true) const override;

  std::shared_ptr<Mesh> GenerateMesh() const override;

public:

  Sphere(glm::vec3 inCenter, float inRadius, std::shared_ptr<Material> inMaterial) :
    center(inCenter), radius(inRadius), ImplicitSurface(inMaterial)
  {}

  glm::vec3 center;
  float radius;
};

#endif
