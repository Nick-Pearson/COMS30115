#ifndef RASTERIZERENDERER_H
#define RASTERIZERENDERER_H

#include "renderer.h"

#include <vector>
#include <memory>

#include <glm/glm.hpp>

class Material;
class RenderTarget;
class Camera;

enum class Axis : uint8_t
{
  X = 0,
  Y,
  Z,
  W
};

class RasterizeRenderer : public Renderer
{
  struct ProjectedVert
  {
    // depth value
    float invdepth;

    // position in homogeneous clip space
    glm::vec4 position;

    void operator-=(const ProjectedVert& other)
    {
      invdepth -= other.invdepth;
      position -= other.position;
    }

    void operator*=(float factor)
    {
      invdepth *= factor;
      position *= factor;
    }

    void operator+=(const ProjectedVert& other)
    {
      invdepth += other.invdepth;
      position += other.position;
    }
  };

public:
  void Draw(const Scene* scene) override;

  /* renders the scene using a given vertex and pixel function
   * @param includePixels :: flag in the template that statically conditonally includes the pixel shader
   * @param Scene         :: scene to render
   * @param Camera        :: camera to render from
   * @param RenderTarget  :: target to render to
   */
  template<bool includePixels = true>
  static void RasterizeScene(const Scene* scene, const Camera* camera, RenderTarget* target);

private:
  static float VertexShader(const glm::mat4& view, const glm::mat4& projection, const glm::vec4& v, glm::vec4& outProjPos);
  static glm::vec3 PixelShader(const Scene* scene, std::shared_ptr<Material> material, const class Triangle& Tri, const struct Vertex& vertexData);

  /* Clips the input list of triangles against the specified Axis
   * @param triangles :: inputs a list of triangles to clip, outputs a list of clipped triangles
   * @param vertexPositions :: inputs positions of the verticies for the triangles in homogenious clip space, outputs new positions
   * @param vertexData :: attributes for each vertex
   * @param axis :: axis to clip against
   */
  static void ClipTriangle(std::vector<Triangle>& inoutTriangles, std::vector<ProjectedVert>& inoutVertexPositions, std::vector<Vertex>& inoutVertexData);

  // this function handles clipping the set of triangles against a single axis
  static void ClipTriangleOnAxis(std::vector<Triangle>& inoutTriangles, std::vector<ProjectedVert>& inoutVertexPositions, std::vector<Vertex>& inoutVertexData, Axis axis);

  // This function handles clipping of a single triangle edge where v0 is valid and v1 is invalid
  static bool ClipLine(const ProjectedVert& v0Pos, const Vertex& v0Data, ProjectedVert& v1Pos, Vertex& v1Data, Axis axis, int sign);

  //performs the perspective divide to convert homogeneous coords to screen space coords
  static glm::ivec2 ConvertHomogeneousCoordinatesToRasterSpace(RenderTarget* target, const glm::vec4& homogeneousCoordinates);

  // returns a projection matrix for the camera transforming points to homogenious clip space
  static glm::mat4 CreatePerspectiveMatrix(const Camera* camera);

  void DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour);
};

#endif
