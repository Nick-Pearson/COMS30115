#ifndef RAYTRACE_RENDERER_H
#define RAYTRACE_RENDERER_H

#include "renderer.h"

#include <glm/glm.hpp>

using glm::vec4;
using glm::mat4;
using glm::vec3;
using glm::mat3;



struct Intersection
{
  vec4 position;
  float distance;
  int triangleIndex;
};

class RaytraceRenderer : public Renderer
{
public:

  virtual void Draw(const std::vector<TestTriangle>& mesh) override;

  bool ClosestIntersection(vec4 start, vec4 dir, const std::vector<TestTriangle>& triangles, Intersection& closestInteraction);
  vec3 DirectLight(const Intersection& intersection, const std::vector<TestTriangle>& mesh);
};

#endif
