#ifndef RASTERIZERENDERER
#define RASTERIZERENDERER

#include "renderer.h"

class RasterizeRenderer : public Renderer
{
public:
  void Draw(const Scene* scene) override;
};

#endif
