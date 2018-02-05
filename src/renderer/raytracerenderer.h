#ifndef RAYTRACE_RENDERER_H
#define RAYTRACE_RENDERER_H

#include "renderer.h"
#include "../scene/scene.h"

#include <glm/glm.hpp>

using glm::vec3;

class RaytraceRenderer : public Renderer
{
public:

  virtual void Draw(const Scene* scene) override;

  vec3 DirectLight(const Intersection& intersection, const Scene* scene);
};

#endif
