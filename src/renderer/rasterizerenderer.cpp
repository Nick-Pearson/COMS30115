#include "rasterizerenderer.h"

#include "../mesh/mesh.h"
#include "../scene/scene.h"
#include "../scene/camera.h"
#include "../amath.h"
#include "../light/light.h"
#include "../material/material.h"
#include "antialiasing.h"

#include <vector>

#include <glm/gtx/transform.hpp>

void RasterizeRenderer::Draw(const Scene* scene)
{
  mat4 cameraMatrix = glm::translate(glm::transpose(scene->camera->rotationMatrix), -scene->camera->position);
  float focalLength = screenptr->width / (2.0f * tan(scene->camera->FOV / TWO_PI));
  const float near = scene->camera->nearClipPlane;
  const float far = scene->camera->farClipPlane;

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

    for(int i = 0; i < V; ++i)
    {
      const float depth = VertexShader(cameraMatrix, focalLength, glm::vec4(mesh->Verticies[i].position, 1.0f), projectedVerts[i].position);
	    projectedVerts[i].invdepth = 1.0f / depth;
    }

    for(int i = 0; i < T; ++i)
    {
      const Triangle& Tri = mesh->Triangles[i];

      // if the entire triangle is behind the camera we can skip it
      if(projectedVerts[Tri.v0].invdepth <= 0.0f &&
        projectedVerts[Tri.v1].invdepth <= 0.0f &&
        projectedVerts[Tri.v2].invdepth <= 0.0f)
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

      if(AMath::isNearlyZero(area))
        continue;

	    for (int y = triMin.y; y <= triMax.y; ++y)
	    {
        const glm::vec2 p(triMin.x-1.0f, y);

        // from : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-practical-implementation
        // the step of w is constant over the x loop so it only needs to be evaluated once at the start of the loop
        float w0_cur = edgeFunction(glm::vec2(v2), glm::vec2(v1), p);
        const float w0_step = v1.y-v2.y;

        float w1_cur = edgeFunction(glm::vec2(v0), glm::vec2(v2), p);
        const float w1_step = v2.y-v0.y;

        float w2_cur = edgeFunction(glm::vec2(v1), glm::vec2(v0), p);
        const float w2_step = v0.y-v1.y;

        bool wasValid = false;

		    for (int x = triMin.x; x <= triMax.x; ++x)
		    {
          // increment our W values
          w0_cur += w0_step;
          w1_cur += w1_step;
          w2_cur += w2_step;

          //check if this point is within the triangle
          if(w0_cur < 0.0f || w1_cur < 0.0f || w2_cur < 0.0f)
          {
            // if we have already drawn the triangle on this line then it is impossible for us to draw it again, so break in that case
            if(!wasValid)
              continue;
            else
              break;
          }

          wasValid = true;

          const float w0 = w0_cur / area;
          const float w1 = w1_cur / area;
          const float w2 = w2_cur / area;

          const float depth = (projectedVerts[Tri.v0].invdepth * w0) + (projectedVerts[Tri.v1].invdepth * w1) + (projectedVerts[Tri.v2].invdepth * w2);

          if (GetDepthSDL(screenptr, x, y) < depth)
          {
            PutDepthSDL(screenptr, x, y, depth);

            Vertex Vert = (mesh->Verticies[Tri.v0] * w0 * projectedVerts[Tri.v0].invdepth) + (mesh->Verticies[Tri.v1] * w1 * projectedVerts[Tri.v1].invdepth) + (mesh->Verticies[Tri.v2] * w2 * projectedVerts[Tri.v2].invdepth);
            Vert *= 1.0f / depth;

            PutFloatPixelSDL(screenptr, x, y, PixelShader(scene, mesh->GetMaterial(i), Tri, Vert));
            //PutFloatPixelSDL(screenptr, x, y, glm::vec3(1.0f, 1.0f, 1.0f) * clamp(depth, 0.0f, 2.55f));
          }
		    }
	    }
    }
  }

  for (int y = 0; y < screenptr->height; y++)
  {
    for (int x = 0; x < screenptr->width; x++)
    {
      //vec3 colour = performAntiAliasing(screenptr->floatBuffer, x, y, screenWidth, screenHeight, screenptr->floatBuffer[y*screenptr->width+x]);
      vec3 colour = glm::vec3(screenptr->floatBuffer[y*screenptr->width + x]);
      PutPixelSDL(screenptr, x, y, colour);
    }
  }
}


float RasterizeRenderer::VertexShader(const glm::mat4& view, float focalLength, const glm::vec4& v, glm::ivec2& outProjPos) const
{
  glm::vec4 transformedV = view * v;

  const float Z = std::abs(transformedV.z);
  outProjPos.x = (int)(focalLength * transformedV.x / Z) + (screenptr->width * 0.5f);
  outProjPos.y = (int)(focalLength * transformedV.y / Z) + (screenptr->height * 0.5f);
  return transformedV.z;
}

glm::vec3 RasterizeRenderer::PixelShader(const Scene* scene, std::shared_ptr<Material> material, const Triangle& Tri, const Vertex& Vertex) const
{
  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    glm::vec3 brdf = material->CalculateBRDF(scene->camera->position - Vertex.position, glm::normalize(light->GetLightDirection(Vertex.position)), Tri.normal);
    colour += brdf * light->CalculateLightAtLocation(Vertex.position);
  }

  return colour;
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
