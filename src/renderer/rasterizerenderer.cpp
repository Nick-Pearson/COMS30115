#include "rasterizerenderer.h"

#include "../mesh/mesh.h"
#include "../scene/scene.h"
#include "../scene/camera.h"
#include "../amath.h"

#include <memory>
#include <vector>

#include <glm/gtx/transform.hpp>

void RasterizeRenderer::Draw(const Scene* scene)
{
  mat4 cameraMatrix = glm::translate(scene->camera->rotationMatrix, scene->camera->position);

  float focalLength = screenptr->width / (2.0f * tan(scene->camera->FOV / TWO_PI));

  const std::vector<std::shared_ptr<Mesh>>* Meshes = scene->GetMeshes();

  for (const std::shared_ptr<Mesh> mesh : *Meshes)
	{
    int V = mesh->Verticies.size();
    int T = mesh->Triangles.size();

    std::vector<ivec2> projectedVerts(V);
    for(int i = 0; i < V; ++i)
    {
      VertexShader(cameraMatrix, focalLength, glm::vec4(mesh->Verticies[i].position, 1.0f), projectedVerts[i]);
    }

    for(int i = 0; i < T; ++i)
    {
      glm::vec3 colour(255.0f, 255.0f, 255.0f);
      const glm::ivec2 v0 = projectedVerts[mesh->Triangles[i].v0];
      const glm::ivec2 v1 = projectedVerts[mesh->Triangles[i].v1];
      const glm::ivec2 v2 = projectedVerts[mesh->Triangles[i].v2];

      std::vector<glm::ivec2> vertices(3);
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;

      int minY = std::min(std::min(v0.y, v1.y), v2.y);
      int maxY = std::max(std::max(v0.y, v1.y), v2.y);
      const int ROWS = maxY - minY;

      if(ROWS <= 0) continue;

      std::vector<int> leftPixels(ROWS);
      std::vector<int> rightPixels(ROWS);

      for(int j = 0; j < ROWS; ++j)
      {
        leftPixels[j] = std::numeric_limits<int>::max();
        rightPixels[j] = -std::numeric_limits<int>::max();
      }

      for (int v = 0; v < 3; ++v) {
        ivec2 a = vertices[v];
        ivec2 b = vertices[(v+1) % 3];

        vec2 maxValue = b - a;
        int maxSize = std::max(std::abs(maxValue.x), std::abs(maxValue.y));
        std::vector<ivec2> results(maxSize);
        AMath::interpolateLine(a, b, results);

        for (int p = 0; p < results.size(); p++)
        {
          const int rowIdx = results[p].y - minY;
          leftPixels[rowIdx] = std::min(leftPixels[rowIdx], results[p].x);
          rightPixels[rowIdx] = std::max(rightPixels[rowIdx], results[p].x);
        }
      }

      for(int y = 0; y < ROWS; ++y)
      {
        for(int x = leftPixels[y]; x < rightPixels[y]; ++x)
        {
          PutPixelSDL(screenptr, x, y + minY, mesh->Triangles[i].colour);
        }
      }
    }
  }
}


void RasterizeRenderer::VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const
{
  glm::vec4 transformedV = view * v;

  outProjPos.x = (int)(focalLength * transformedV.x / transformedV.z) + (screenptr->width * 0.5f);
  outProjPos.y = (int)(focalLength * transformedV.y / transformedV.z) + (screenptr->height * 0.5f);
}

void RasterizeRenderer::DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour)
{
  vec2 maxValue = b - a;
  int maxSize = std::max(std::abs(maxValue.x), std::abs(maxValue.y));
  std::vector<ivec2> points(maxSize);

  AMath::interpolate(a, b, points);

  for (int i = 0; i < points.size(); ++i) {
      PutPixelSDL(screenptr, points[i].x, points[i].y, colour);
  }
}
