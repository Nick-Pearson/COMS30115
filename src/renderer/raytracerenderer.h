#ifndef RAYTRACE_RENDERER_H
#define RAYTRACE_RENDERER_H

#include "renderer.h"

#include <glm/glm.hpp>

using glm::vec3;

struct Intersection;

class RaytraceRenderer : public Renderer
{
public:

  virtual void Draw(const Scene* scene) override;


  vec3 ShadePoint(const vec3& position, const vec3& dir, const Scene* scene);

private:
  
  vec3 ShadePoint_Internal(const vec3& position, const vec3& dir, const Scene* scene, int curDepth, Intersection& intersection);

  vec3 DirectLight(const vec3& src_position, const Intersection& intersection, const Scene* scene);
};

#endif
