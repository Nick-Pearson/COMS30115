#ifndef RASTERIZERENDERER_H
#define RASTERIZERENDERER_H

#include "renderer.h"

#include <memory>

#include <glm/glm.hpp>

class Material;
class RenderTarget;
class Camera;

enum class Outcode : uint8_t
{
  INSIDE    = 0, // 0000
  LEFT      = 1, // 0001
  RIGHT     = 2, // 0010
  BOTTOM    = 4, // 0100
  TOP       = 8  // 1000
};

inline Outcode& operator|=(Outcode& a, Outcode b)
{
  return (Outcode&)((uint8_t&)a |= (uint8_t)b);
}

inline Outcode operator&(Outcode a, Outcode b)
{
  return (Outcode)((uint8_t)a & (uint8_t)b);
}

class RasterizeRenderer : public Renderer
{
public:
  void Draw(const Scene* scene) override;

  /* renders the scene using a given vertex and pixel function
   * @param includePixels :: flag in the template that statically conditonally includes the pixel shader
   * @param Scene         :: scene to render
   * @param RenderTarget  :: target to render to
   * @param VertexShader  :: function that transforms a vertex position to homogenious clip space
   * @param PixelShader   :: function that calculates a colour value from material and triangle/vertex values
   */
  template<bool includePixels = true, typename VertexPred, typename PixelPred>
  static void RasterizeScene(const Scene* scene, RenderTarget* screenptr, VertexPred VertexShader, PixelPred PixelShader);

private:
  float VertexShader(const glm::mat4& view, const glm::mat4& projection, const glm::vec4& v, glm::vec4& outProjPos) const;
  glm::vec3 PixelShader(const Scene* scene, std::shared_ptr<Material> material, const class Triangle& Tri, const struct Vertex& Vertex) const;

  static Outcode CalculateOutcode(RenderTarget* target, int x, int y);

  // returns a projection matrix for the camera transforming points to homogenious clip space
  glm::mat4 CreatePerspectiveMatrix(const Camera* camera) const;

  void DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour);
};

#endif
