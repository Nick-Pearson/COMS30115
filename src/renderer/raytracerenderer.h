#ifndef RAYTRACE_RENDERER_H
#define RAYTRACE_RENDERER_H

#include "renderer.h"

class RaytraceRenderer : public Renderer
{
public:

  RaytraceRenderer(int ScreenWidth, int ScreenHeight) :
    Renderer(ScreenWidth, ScreenHeight)
  {}

  virtual void Draw(const std::vector<TestTriangle>& mesh) override;

};

#endif
