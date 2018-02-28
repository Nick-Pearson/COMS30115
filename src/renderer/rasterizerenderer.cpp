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
  mat4 cameraMatrix = glm::translate(glm::transpose(scene->camera->rotationMatrix), -scene->camera->position);
  float focalLength = screenptr->width / (2.0f * tan(scene->camera->FOV / TWO_PI));

  const std::vector<std::shared_ptr<Mesh>>* Meshes = scene->GetMeshes();

  for (const std::shared_ptr<Mesh> mesh : *Meshes)
  {
    int V = mesh->Verticies.size();
    int T = mesh->Triangles.size();

	  struct ProjectedVert
	  {
		  float invdepth;
		  glm::ivec2 position;
	  };
    std::vector<ProjectedVert> projectedVerts(V);

    #pragma omp parallel for 
    for(int i = 0; i < V; ++i)
    {
	    projectedVerts[i].invdepth = 1.0f / VertexShader(cameraMatrix, focalLength, glm::vec4(mesh->Verticies[i].position, 1.0f), projectedVerts[i].position);
    }

    for(int i = 0; i < T; ++i)
    {
      const Triangle& Tri = mesh->Triangles[i];

      // if the entire triangle is behind the camera we can skip it
      if(projectedVerts[Tri.v0].invdepth < 0.0f &&
        projectedVerts[Tri.v1].invdepth < 0.0f &&
        projectedVerts[Tri.v2].invdepth < 0.0f)
        continue;

      const glm::ivec2& v0 = projectedVerts[Tri.v0].position;
      const glm::ivec2& v1 = projectedVerts[Tri.v1].position;
      const glm::ivec2& v2 = projectedVerts[Tri.v2].position;

      // coordinates of the triangle
      glm::ivec2 triMin, triMax;
      triMin.x = std::max(std::min(std::min(v0.x, v1.x), v2.x), 0);
      triMin.y = std::max(std::min(std::min(v0.y, v1.y), v2.y), 0);
      triMax.x = std::min(std::max(std::max(v0.x, v1.x), v2.x), screenptr->width-1);
      triMax.y = std::min(std::max(std::max(v0.y, v1.y), v2.y), screenptr->height-1);

      auto edgeFunction = [](const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& p)
      {
        return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
      };

      const float area = edgeFunction(glm::vec2(v1), glm::vec2(v0), glm::vec2(v2));

      #pragma omp parallel for
	    for (int y = triMin.y; y <= triMax.y; ++y)
	    {
		    for (int x = triMin.x; x <= triMax.x; ++x)
		    {
          const glm::vec2 p(x, y);

          float w1 = edgeFunction(glm::vec2(v0), glm::vec2(v2), p);
          float w0 = edgeFunction(glm::vec2(v2), glm::vec2(v1), p);
          float w2 = edgeFunction(glm::vec2(v1), glm::vec2(v0), p);

          //check if this point is within the triangle
          if(w0 < 0.0f || w1 < 0.0f || w2 < 0.0f)
            continue;

          w0 /= area;
          w1 /= area;
          w2 /= area;

          const float depth = (projectedVerts[Tri.v0].invdepth * w0) + (projectedVerts[Tri.v1].invdepth * w1) + (projectedVerts[Tri.v2].invdepth * w2);

          if(GetDepthSDL(screenptr, x, y) > depth)
            continue;

          PutDepthSDL(screenptr, x, y, depth);
			    PutPixelSDL(screenptr, x, y, Tri.colour * 255.0f);
          //PutPixelSDL(screenptr, x, y, glm::vec3(100.0f, 100.0f, 100.0f) * clamp(depth, 0.0f, 2.55f));
		    }
	    }
    }
  }
}


float RasterizeRenderer::VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const
{
  glm::vec4 transformedV = view * v;

  outProjPos.x = (int)(focalLength * transformedV.x / transformedV.z) + (screenptr->width * 0.5f);
  outProjPos.y = (int)(focalLength * transformedV.y / transformedV.z) + (screenptr->height * 0.5f);
  return transformedV.z;
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
