#include "raytracerenderer.h"

#include "../scene/camera.h"
#include "../mesh/mesh.h"

#include <omp.h>
#include <math.h>


#include <glm/gtx/norm.hpp>

using glm::vec4;
using glm::mat4;
using glm::mat3;

void RaytraceRenderer::Draw(const Scene* scene)
{
  float focalLength = scene->camera->focalLength;

  int screenWidth = screenptr->width;
  int screenHeight = screenptr->height;
  int sceenWidthHalf = screenWidth * 0.5;
  int screenHeightHalf = screenHeight * 0.5;
  mat4 rotationMatrix = scene->camera->rotationMatrix;
  vec3 cameraPosition = scene->camera->position;

  #pragma omp parallel for schedule(static)
  for (int y = 0; y < screenHeight; y++)
  {
    for (int x = 0; x < screenWidth; x++)
    {
      vec4 dir = rotationMatrix * vec4(x - sceenWidthHalf, y - screenHeightHalf, focalLength, 1);

      Intersection closestIntersection;

      if (scene->ClosestIntersection(cameraPosition, vec3(dir), closestIntersection))
      {
        vec3 lightColour = DirectLight(closestIntersection, scene);
        vec3 colour = lightColour * closestIntersection.mesh->Triangles[closestIntersection.triangleIndex].colour;


        PutPixelSDL(screenptr, x, y, colour);

      }
    }
  }
}

vec3 RaytraceRenderer::DirectLight(const Intersection& intersection, const Scene* scene)
{
  vec3 lightPos( 0, -0.5, -0.7 );
  vec3 lightColor = 14.f * vec3( 1, 1, 1 );

  {
    Intersection shadowIntersection;
    vec3 shadowDir = lightPos - vec3(intersection.position);
    if(scene->ClosestIntersection(intersection.position, shadowDir, shadowIntersection))
    {
      if(shadowIntersection.distance < 1.0f)
        return vec3(0, 0, 0);
    }
  }

  vec3 rHat = lightPos - vec3(intersection.position);
  float d_sqrd = glm::length2(rHat);
  rHat = glm::normalize(rHat);

  float a = 4.0f * M_PI * d_sqrd;

  float rAndN = glm::dot(intersection.mesh->Triangles[intersection.triangleIndex].normal, rHat);

  if (rAndN < 0)
    return vec3(0,0,0);

  return rAndN * lightColor / a;
}
