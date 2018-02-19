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
      const glm::ivec2 v0 = projectedVerts[mesh->Triangles[i].v0];
      const glm::ivec2 v1 = projectedVerts[mesh->Triangles[i].v1];
      const glm::ivec2 v2 = projectedVerts[mesh->Triangles[i].v2];

	  // coordinates of the triangle
	  glm::ivec2 triMin, triMax;
      triMin.x = std::max(std::min(std::min(v0.x, v1.x), v2.x), 0);
	  triMin.y = std::max(std::min(std::min(v0.y, v1.y), v2.y), 0);
	  triMax.x = std::min(std::max(std::max(v0.x, v1.x), v2.x), screenptr->width);
	  triMax.y = std::min(std::max(std::max(v0.y, v1.y), v2.y), screenptr->height);

#pragma omp parallel for
	  for (int y = triMin.y; y <= triMax.y; ++y)
	  {
		  for (int x = triMin.x; x <= triMax.x; ++x)
		  {
			  //check if this point is within the triangle
			  if(!AMath::IsPointWithinTriangle(ivec2(x,y), v0, v1, v2))
				  continue;

			  PutPixelSDL(screenptr, y, x, mesh->Triangles[i].colour * 255.0f);
		  }
	  }

    }
  }
}


void RasterizeRenderer::VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const
{
  glm::vec4 transformedV = view * v;

  outProjPos.x = (int)(focalLength * transformedV.x / -transformedV.z) + (screenptr->width * 0.5f);
  outProjPos.y = (int)(focalLength * transformedV.y / -transformedV.z) + (screenptr->height * 0.5f);
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
