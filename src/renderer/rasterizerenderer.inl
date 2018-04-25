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

    for (int triIdx = 0; triIdx < V; ++triIdx)
    {
      projectedVerts[triIdx].invdepth = 1.0f / VertexShader(cameraMatrix, projection, glm::vec4(mesh->Verticies[triIdx].position, 1.0f), projectedVerts[triIdx].position);
    }

    for (int triIdx = 0; triIdx < T; ++triIdx)
    {
      Triangle& triangle = mesh->Triangles[triIdx];

      //backface culling
      const glm::vec3 trianglePos = (mesh->Verticies[triangle.v0].position + mesh->Verticies[triangle.v1].position + mesh->Verticies[triangle.v2].position) / 3.0f;
      const glm::vec3 cameraVector = trianglePos - camera->position;
      if (glm::dot(triangle.normal, cameraVector) >= 0.0f)
      {
        continue;
      }

      std::vector<Triangle> clippedTriangles({ mesh->Triangles[triIdx] });

      std::vector<ProjectedVert> clippedVerticies({
        projectedVerts[triangle.v0],
        projectedVerts[triangle.v1],
        projectedVerts[triangle.v2] });

      std::vector<Vertex> clippedVertexData({
        mesh->Verticies[triangle.v0],
        mesh->Verticies[triangle.v1],
        mesh->Verticies[triangle.v2] });

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

        const int maxY = std::max(v0.y, std::max(v1.y, v2.y));
        const int minY = std::min(v0.y, std::min(v1.y, v2.y));
        const int rows = 1 + maxY - minY;

        if (rows == 0) continue;

        //iterable list of verts and triangle indices
        const glm::ivec2 verts[3] = { v0, v1, v2 };
        const int indicies[3] = { Tri.v0, Tri.v1, Tri.v2 };

        for (int rowIdx = (minY < 0 ? -minY : 0); rowIdx < rows; ++rowIdx)
        {
          const int pixelY = minY + rowIdx;
          if(pixelY >= target->width)
            break;

          // coordinates for this row
          int leftPixel = target->width - 1;
          int rightPixel = 0;

          // vertex data at the left and right pixels
          Vertex leftVertexData, rightVertexData;
          float leftInvDepth = 0.0f, rightInvDepth = 0.0f;

          for (int vi = 0; vi < 3; ++vi)
          {
            glm::ivec2 p0 = verts[vi];
            glm::ivec2 p1 = verts[(vi + 1) % 3];
            bool swapped = false;

            if (p1.y < p0.y)
            {
              std::swap(p0, p1);
              swapped = true;
            }

            //check the Y value is in range for this edge
            if(pixelY < p0.y || pixelY > p1.y) continue;

            int xVal = p0.x;

            float q = (float)xVal / (float)(1.0f + p1.x - p0.x);

            // check to prevent divide by zero weirdness
            if (p1.y != p0.y)
            {
              // x change for 1 change in y
              const float yDiff = pixelY - p0.y;
              const float slope = (float)(p1.x - p0.x) / (float)(p1.y - p0.y);

              xVal = (float)p0.x + (slope * yDiff);

              q = yDiff / (float)(p1.y - p0.y);
            }

            int idx1 = indicies[vi];
            int idx2 = indicies[(vi + 1) % 3];

            if(swapped) std::swap(idx1, idx2);

            float invDepth = (clippedVerticies[idx1].invdepth * (1.0f - q)) + (clippedVerticies[idx2].invdepth * q);
            Vertex vertexData = (clippedVertexData[idx1] * clippedVerticies[idx1].invdepth * (1.0f - q)) +
              (clippedVertexData[idx2] * clippedVerticies[idx2].invdepth * q);
            vertexData *= 1.0f / invDepth;

            if (leftPixel > xVal && xVal >= 0)
            {
              leftPixel = xVal;
              leftInvDepth = invDepth;
              leftVertexData = vertexData;
            }

            if (rightPixel < xVal && xVal < target->width)
            {
              rightPixel = xVal;
              rightInvDepth = invDepth;
              rightVertexData = vertexData;
            }
          }

          // fill the row
          for (int pixelX = leftPixel; pixelX < rightPixel; ++pixelX)
          {
            float q = (float)(pixelX - leftPixel) / (float)(1 + rightPixel - leftPixel);

            float invDepth = (leftInvDepth * (1.0f - q)) + (rightInvDepth * q);

            if (target->GetDepth(pixelX, pixelY) <= invDepth)
            {
              target->PutDepth(pixelX, pixelY, invDepth);

              if (includePixels)
              {
                Vertex Vert = (leftVertexData * leftInvDepth * (1.0f - q)) + (rightVertexData * rightInvDepth * q);
                Vert *= 1.0f / invDepth;

                target->PutFloatPixel(pixelX, pixelY, PixelShader(scene, mesh->GetMaterial(triIdx), Tri, Vert));
              }
            }
          }
        }
      }
    }
  }
}

#endif
