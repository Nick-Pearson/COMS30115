#ifndef IMPLICITSURFACE_H
#define IMPLICITSURFACE_H

#include "../scene/scene.h"

#include <glm/glm.hpp>

#include <memory>

class Mesh;
class Material;

class ImplicitSurface
{
public:

  // raytrace against the surface
  virtual bool Intersect(const glm::vec3& start, const glm::vec3& dir, float& outDistance, glm::vec3& outNormal) const = 0;

  // convert this surface to a triangle mesh
  virtual std::shared_ptr<Mesh> GenerateMesh() const = 0;

  inline virtual std::shared_ptr<Material> GetMaterial() const { return material; }

public:

  ImplicitSurface(std::shared_ptr<Material> inMaterial) :
    material(inMaterial)
  {}

  std::shared_ptr<Material> material;
};

#endif
