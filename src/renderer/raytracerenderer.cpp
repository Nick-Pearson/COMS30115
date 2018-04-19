#include "raytracerenderer.h"

#include "../scene/camera.h"
#include "../scene/scene.h"
#include "../mesh/mesh.h"
#include "../texture/texture.h"
#include "../material/material.h"
#include "../amath.h"
#include "../light/light.h"

#include "antialiasing.h"

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
#define MAX_BOUNCES 0
#define NUM_DIRS 128

// number of shadows rays to send out
#define SHADOW_RAYS 1

// takes around 10 secs for 720x720
//#define MAX_BOUNCES 0
//#define NUM_DIRS 4

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

  float focalLength = screenWidth / tan(AMath::ToRads(scene->camera->FOV));

  #pragma omp parallel for schedule(static)
  for (int y = 0; y < screenHeight; y++)
  {
    for (int x = 0; x < screenWidth; x++)
    {
      vec4 dir = rotationMatrix * vec4(x - sceenWidthHalf, y - screenHeightHalf, focalLength, 1);

      vec3 colour = ShadePoint(cameraPosition, vec3(dir), scene);

      screenptr->PutFloatPixel(x, y, colour);
    }
  }

  for (int y = 0; y < screenHeight; y++)
  {
    for (int x = 0; x < screenWidth; x++)
    {
      vec3 colour = performAntiAliasing(screenptr->floatBuffer, x, y, screenptr->width, screenptr->height);
      screenptr->PutPixel(x, y, colour);
    }
  }
}

vec3 RaytraceRenderer::DirectLight(const vec3& src_position, const Intersection& intersection, const Scene* scene)
{
  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    float lightContribution = 0.0f;
    vec3 lightDir = light->GetLightDirection(vec3(intersection.position));

    if(light->CastsShadows())
    {
      for(int r = 0; r < SHADOW_RAYS; ++r)
      {
        vec3 randLightDir = light->GetRandomLightDirection(vec3(intersection.position));

        if(r == 0) //ensure at least one ray goes directly to the light
          randLightDir = lightDir;

        Intersection shadowIntersection;
        if (scene->ShadowIntersection(intersection.position, randLightDir, shadowIntersection))
        {
          continue;
        }

        lightContribution += (1.0f / (float)SHADOW_RAYS);
      }
    }
    else
    {
      lightContribution = 1.0f;
    }

    glm::vec3 brdf = intersection.mesh->GetMaterial(intersection.triangleIndex)->CalculateBRDF(src_position - intersection.position, glm::normalize(lightDir), intersection.mesh->Triangles[intersection.triangleIndex].normal);
    colour += lightContribution * brdf * light->CalculateLightAtLocation(intersection.position);
  }

  return colour;
}

vec3 RaytraceRenderer::ShadePoint(const vec3& position, const vec3& dir, const Scene* scene)
{
  Intersection outIntersection;
  return ShadePoint_Internal(position, dir, scene, MAX_BOUNCES, outIntersection);
}

vec3 RaytraceRenderer::ShadePoint_Internal(const vec3& position, const vec3& dir, const Scene* scene, int curDepth, Intersection& intersection)
{
  //if (!scene->ClosestIntersection(position, dir, intersection))
  //  return scene->GetEnvironmentColour(dir);
  if (!scene->ClosestIntersection(position, dir, intersection))
    return vec3(0.0f, 0.0f, 0.0f);

  vec3 light = DirectLight(position, intersection, scene);

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

	  Intersection indIntersection;
	  vec3 indColour = ShadePoint_Internal(intersection.position, indir_dir, scene, curDepth - 1, indIntersection);
	  float distanceFactor = 1.0f;
	  float directionFactor = 1.0f;

	  if(indIntersection.distance > 0.0f)
		 distanceFactor = 1.5f / (indIntersection.distance * indIntersection.distance);

	  if (indIntersection.mesh != nullptr)
		 directionFactor = std::abs(glm::dot(indIntersection.mesh->Triangles[indIntersection.triangleIndex].normal, triangleNormal));

	  indirectLight += lightFactor * directionFactor * indColour * distanceFactor;
    }

    light += (indirectLight / (float)NUM_DIRS);
  }

  return light;
}
