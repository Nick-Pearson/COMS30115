#ifndef RASTERIZERENDERER
#define RASTERIZERENDERER

#include "renderer.h"

#include <glm/glm.hpp>

class RasterizeRenderer : public Renderer
{
public:
  void Draw(const Scene* scene) override;

private:
  void VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const;

  void DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour);
};

#endif
