#ifndef RASTERIZERENDERER
#define RASTERIZERENDERER

#include "renderer.h"

#include <memory>

#include <glm/glm.hpp>

class Material;

class RasterizeRenderer : public Renderer
{
public:
  void Draw(const Scene* scene) override;

private:
  float VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const;
  glm::vec3 PixelShader(const Scene* scene, std::shared_ptr<Material> material, const class Triangle& Tri, const struct Vertex& Vertex) const;

  void DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour);
};

#endif
