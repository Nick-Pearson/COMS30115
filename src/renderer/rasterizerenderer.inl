#ifndef RASTERIZERENDERER_INL
#define RASTERIZERENDERER_INL

#include "../mesh/mesh.h"
#include "../scene/scene.h"
#include "../scene/camera.h"
#include "../amath.h"
#include "../misc.h"
#include "../light/light.h"
#include "../material/material.h"
#include "antialiasing.h"
#include "rendertarget.h"

#include <glm/gtx/transform.hpp>

template<bool includePixels>
void RasterizeRenderer::RasterizeScene(const Scene* scene, const Camera* camera, RenderTarget* target)
{
  const mat4 cameraMatrix = glm::translate(glm::transpose(camera->rotationMatrix), -camera->position);
  const mat4 projection = CreatePerspectiveMatrix(camera);

  const std::vector<std::shared_ptr<Mesh>>* Meshes = scene->GetMeshes();

  for (const std::shared_ptr<Mesh> mesh : *Meshes)
  {
    size_t V = mesh->Verticies.size();
    size_t T = mesh->Triangles.size();
    std::vector<ProjectedVert> projectedVerts(V);

    for (int i = 0; i < V; ++i)
    {
      projectedVerts[i].depth = VertexShader(cameraMatrix, projection, glm::vec4(mesh->Verticies[i].position, 1.0f), projectedVerts[i].position);
    }

    for (int i = 0; i < T; ++i)
    {
      std::vector<Triangle> clippedTriangles({ mesh->Triangles[i] });

      std::vector<ProjectedVert> clippedVerticies({
        projectedVerts[clippedTriangles[0].v0],
        projectedVerts[clippedTriangles[0].v1],
        projectedVerts[clippedTriangles[0].v2] });

      std::vector<Vertex> clippedVertexData({
        mesh->Verticies[clippedTriangles[0].v0],
        mesh->Verticies[clippedTriangles[0].v1],
        mesh->Verticies[clippedTriangles[0].v2] });

      // remap the indexes to be relative to the new coordinate list we have constructed
      clippedTriangles[0].v0 = 0;
      clippedTriangles[0].v1 = 1;
      clippedTriangles[0].v2 = 2;

      ClipTriangle(clippedTriangles, clippedVerticies, clippedVertexData);

      for (const Triangle& Tri : clippedTriangles)
      {
        const glm::ivec2 v0 = ConvertHomogeneousCoordinatesToRasterSpace(target, clippedVerticies[Tri.v0].position);
        const glm::ivec2 v1 = ConvertHomogeneousCoordinatesToRasterSpace(target, clippedVerticies[Tri.v1].position);
        const glm::ivec2 v2 = ConvertHomogeneousCoordinatesToRasterSpace(target, clippedVerticies[Tri.v2].position);

        // coordinates of the triangle
        glm::ivec2 triMin, triMax;
        triMin.x = std::max(std::min(std::min(v0.x, v1.x), v2.x), 0);
        triMin.y = std::max(std::min(std::min(v0.y, v1.y), v2.y), 0);
        triMax.x = std::min(std::max(std::max(v0.x, v1.x), v2.x), target->width - 1);
        triMax.y = std::min(std::max(std::max(v0.y, v1.y), v2.y), target->height - 1);

        auto edgeFunction = [](const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& p)
        {
          return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
        };

        const float area = edgeFunction(glm::vec2(v1), glm::vec2(v0), glm::vec2(v2));

        if (AMath::isNearlyZero(area))
          continue;

        for (int y = triMin.y; y <= triMax.y; ++y)
        {
          const glm::vec2 p(triMin.x - 1.0f, y);

          // from : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-practical-implementation
          // the step of w is constant over the x loop so it only needs to be evaluated once at the start of the loop
          float w0_cur = edgeFunction(glm::vec2(v2), glm::vec2(v1), p);
          const float w0_step = v1.y - v2.y;

          float w1_cur = edgeFunction(glm::vec2(v0), glm::vec2(v2), p);
          const float w1_step = v2.y - v0.y;

          float w2_cur = edgeFunction(glm::vec2(v1), glm::vec2(v0), p);
          const float w2_step = v0.y - v1.y;

          bool wasValid = false;

          for (int x = triMin.x; x <= triMax.x; ++x)
          {
            // increment our W values
            w0_cur += w0_step;
            w1_cur += w1_step;
            w2_cur += w2_step;

            //check if this point is within the triangle
            if (w0_cur < 0.0f || w1_cur < 0.0f || w2_cur < 0.0f)
            {
              // if we have already drawn the triangle on this line then it is impossible for us to draw it again, so break in that case
              if (!wasValid)
                continue;
              else
                break;
            }

            wasValid = true;

            const float w0 = w0_cur / area;
            const float w1 = w1_cur / area;
            const float w2 = w2_cur / area;

            const float invdepth0 = 1.0f / clippedVerticies[Tri.v0].depth;
            const float invdepth1 = 1.0f / clippedVerticies[Tri.v1].depth;
            const float invdepth2 = 1.0f / clippedVerticies[Tri.v2].depth;

            const float depth = (invdepth0 * w0) + (invdepth1 * w1) + (invdepth2 * w2);

            if (target->GetDepth(x, y) < depth)
            {
              target->PutDepth(x, y, depth);

              if (includePixels)
              {
                Vertex Vert = (clippedVertexData[Tri.v0] * w0 * invdepth0) + (clippedVertexData[Tri.v1] * w1 * invdepth1) + (clippedVertexData[Tri.v2] * w2 * invdepth2);
                Vert *= 1.0f / depth;

                target->PutFloatPixel(x, y, PixelShader(scene, mesh->GetMaterial(i), Tri, Vert));
              }
            }
          }
        }
      }
    }
  }
}


#endif
