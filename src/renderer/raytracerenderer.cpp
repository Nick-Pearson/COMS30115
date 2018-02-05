#include "raytracerenderer.h"

#include "../scene/camera.h"
#include "../scene/scene.h"
#include "../mesh/mesh.h"

#include <iostream>
#include <random>

#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

// no indirect light
#define MAX_BOUNCES 0
#define NUM_DIRS 0

// takes around 4 mins for 720x720
//#define MAX_BOUNCES 2
//#define NUM_DIRS 16

// takes around 10 secs for 720x720
//#define MAX_BOUNCES 2
//#define NUM_DIRS 4


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

      vec3 colour = ShadePoint(scene->camera->position, vec3(dir), scene);
      PutPixelSDL(screenptr, x, y, colour);
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

vec3 RaytraceRenderer::ShadePoint(const vec3& position, const vec3& dir, const Scene* scene)
{
  return ShadePoint_Internal(position, dir, scene, MAX_BOUNCES);
}

vec3 RaytraceRenderer::ShadePoint_Internal(const vec3& position, const vec3& dir, const Scene* scene, int curDepth)
{
  Intersection intersection;
  if (!scene->ClosestIntersection(position, dir, intersection))
    return vec3(0.0f, 0.0f, 0.0f);

  vec3 light = DirectLight(intersection, scene);

  if(curDepth > 0)
  {
    vec3 indirectLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 triangleNormal = intersection.mesh->Triangles[intersection.triangleIndex].normal;

    for(int i = 0; i < NUM_DIRS; ++i)
    {
      // add a random direction of indirect light
      const float theta1 = (float)rand() * 3.14f / (float)RAND_MAX;
      const float theta2 = (float)rand() * 3.14f / (float)RAND_MAX;
      const float theta3 = (float)rand() * 3.14f / (float)RAND_MAX;

      glm::mat4 rotationMatrix;
      rotationMatrix = glm::rotate(rotationMatrix, theta1, glm::vec3(1.0f, 0.0f, 0.0f));
      rotationMatrix = glm::rotate(rotationMatrix, theta2, glm::vec3(0.0f, 1.0f, 0.0f));
      rotationMatrix = glm::rotate(rotationMatrix, theta3, glm::vec3(0.0f, 0.0f, 1.0f));

      const glm::vec3 dir = vec3(vec4(triangleNormal, 1.0f) * rotationMatrix);
      //const float lightFactor = glm::dot(dir, triangleNormal);

      indirectLight += ShadePoint_Internal(intersection.position, dir, scene, curDepth - 1);
    }

    light += (indirectLight / (float)NUM_DIRS);
  }

  return light * intersection.mesh->Triangles[intersection.triangleIndex].colour;
}
