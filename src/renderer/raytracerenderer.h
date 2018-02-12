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


  vec3 DirectLight(const Intersection& intersection, const Scene* scene);
  vec3 ShadePoint(const vec3& position, const vec3& dir, const Scene* scene);

private:
  void PutPixelBuffer(vec3 *buffer, int x, int y, int width, int height, glm::vec3 colour);
  vec3 ShadePoint_Internal(const vec3& position, const vec3& dir, const Scene* scene, int curDepth);
};

#endif
