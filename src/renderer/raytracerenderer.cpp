#include "raytracerenderer.h"

#include "../scene/camera.h"
#include "../mesh/mesh.h"

#include <glm/gtx/norm.hpp>

using glm::vec4;
using glm::mat4;
using glm::mat3;

void RaytraceRenderer::Draw(const Scene* scene)
{
  float focalLength = scene->camera->focalLength;

  for (int y = 0; y < screenptr->height; y++)
  {
    for (int x = 0; x < screenptr->width; x++)
    {
      vec4 dir = scene->camera->rotationMatrix * vec4(x - (screenptr->width / 2), y - (screenptr->height / 2), focalLength, 1);

      Intersection closestIntersection;

      if (scene->ClosestIntersection(scene->camera->position, vec3(dir), closestIntersection))
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

  float a = 4.0f * 3.14f * d_sqrd;

  float rAndN = glm::dot(intersection.mesh->Triangles[intersection.triangleIndex].normal, rHat);

  if (rAndN < 0)
    return vec3(0,0,0);

  return rAndN * lightColor / a;
}
