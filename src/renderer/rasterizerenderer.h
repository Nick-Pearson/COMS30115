#ifndef RASTERIZERENDERER
#define RASTERIZERENDERER

#include "renderer.h"

#include <memory>

#include <glm/glm.hpp>

class Material;
class RenderTarget;

class RasterizeRenderer : public Renderer
{
public:
  void Draw(const Scene* scene) override;

  // renders the scene using a given vertex and pixel function
  template<bool includePixels = true, typename VertexPred, typename PixelPred>
  static void RasterizeScene(const Scene* scene, RenderTarget* screenptr, VertexPred VertexShader, PixelPred PixelShader);

private:
  float VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const;
  glm::vec3 PixelShader(const Scene* scene, std::shared_ptr<Material> material, const class Triangle& Tri, const struct Vertex& Vertex) const;

  void DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour);
};

#endif
