#include "raytracerenderer.h"

#include "../scene/camera.h"
#include "../scene/scene.h"
#include "../mesh/mesh.h"
#include "../texture/texture.h"
#include "../material/material.h"

#include <iostream>
#include <random>
#include <omp.h>
#include <math.h>

#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

// no indirect light
//#define MAX_BOUNCES 0
//#define NUM_DIRS 0

// takes around 4 mins for 720x720
#define MAX_BOUNCES 2
#define NUM_DIRS 16

// takes around 10 secs for 720x720
//#define MAX_BOUNCES 0
//#define NUM_DIRS 4


#define TWO_PI 6.283185f

using glm::vec4;
using glm::mat4;
using glm::mat3;

void RaytraceRenderer::Draw(const Scene* scene)
{
  int screenWidth = screenptr->width;
  int screenHeight = screenptr->height;
  int sceenWidthHalf = (int)(screenWidth * 0.5f);
  int screenHeightHalf = (int)(screenHeight * 0.5f);
  mat4 rotationMatrix = scene->camera->rotationMatrix;
  vec3 cameraPosition = scene->camera->position;

  float focalLength = screenWidth / (2.0f * tan(scene->camera->FOV / TWO_PI));

  #pragma omp parallel for schedule(static)
  for (int y = 0; y < screenHeight; y++)
  {
    for (int x = 0; x < screenWidth; x++)
    {
      vec4 dir = rotationMatrix * vec4(x - sceenWidthHalf, y - screenHeightHalf, focalLength, 1);

      vec3 colour = ShadePoint(cameraPosition, vec3(dir), scene);
      PutPixelSDL(screenptr, x, y, colour);
    }
  }
}

vec3 RaytraceRenderer::DirectLight(const vec3& view, const Intersection& intersection, const Scene* scene)
{
  vec3 lightPos( 0, -0.5, -0.7 );
  vec3 lightColor = 400.f * vec3( 1, 1, 1 );

  {
    Intersection shadowIntersection;
    vec3 shadowDir = lightPos - vec3(intersection.position);
    if(scene->ShadowIntersection(intersection.position, shadowDir, shadowIntersection))
    {
      return vec3(0, 0, 0);
    }
  }

  vec3 rHat = lightPos - vec3(intersection.position);
  float d_sqrd = glm::length2(rHat);
  rHat = glm::normalize(rHat);

  float a = 4.0f * (float)M_PI * d_sqrd;

  float brdf = Material::CalculateBRDF(view, rHat, intersection.mesh->Triangles[intersection.triangleIndex].normal);

  if (brdf < 0)
    return vec3(0,0,0);

  return brdf * lightColor / a;
}

vec3 RaytraceRenderer::ShadePoint(const vec3& position, const vec3& dir, const Scene* scene)
{
  return ShadePoint_Internal(position, dir, scene, MAX_BOUNCES);
}

vec3 RaytraceRenderer::ShadePoint_Internal(const vec3& position, const vec3& dir, const Scene* scene, int curDepth)
{
  Intersection intersection;
  if (!scene->ClosestIntersection(position, dir, intersection))
    return scene->GetEnvironmentColour(dir);

  vec3 light = DirectLight(dir, intersection, scene);

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

      const glm::vec3 indir_dir = vec3(vec4(triangleNormal, 1.0f) * rotationMatrix);
      const float lightFactor = glm::dot(indir_dir, triangleNormal);

  	  if (lightFactor <= 0.0f)
  		  continue;

      indirectLight += lightFactor * ShadePoint_Internal(intersection.position, indir_dir, scene, curDepth - 1);
    }

    light += (indirectLight / (float)NUM_DIRS);
  }

  return light * intersection.mesh->Triangles[intersection.triangleIndex].colour;
}
