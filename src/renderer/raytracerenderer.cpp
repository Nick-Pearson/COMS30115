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
#define MAX_BOUNCES 5
#define NUM_DIRS 1000

// if set to 0 then simple shading is used
#define USE_GI 0

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

  for (int y = 0; y < screenHeight; y++)
  {
    for (int x = 0; x < screenWidth; x++)
    {
      vec4 dir = rotationMatrix * vec4(x - sceenWidthHalf, y - screenHeightHalf, focalLength, 1);

      vec3 colour = ShadePoint(cameraPosition, vec3(dir), scene);

      screenptr->PutFloatPixel(x, y, colour);
    }

#if USE_GI
    std::cout << "Done row " << y << std::endl;
#endif
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

vec3 RaytraceRenderer::ShadePoint(const vec3& position, const vec3& dir, const Scene* scene)
{
#if USE_GI
  vec3 totalcolour;

  #pragma omp parallel for
  for (int i = 0; i < NUM_DIRS; ++i)
  {
    Intersection outIntersection;
    vec3 colour = ShadePoint_Internal(position, dir, scene, MAX_BOUNCES, outIntersection);

    #pragma omp critical
    totalcolour += colour;
  }

  return totalcolour / (float)NUM_DIRS;
#else
  Intersection intersection;
  if (!scene->ClosestIntersection(position, dir, intersection))
    return vec3(0.0f, 0.0f, 0.0f);

  return DirectLight(position, intersection, scene);
#endif
}

vec3 RaytraceRenderer::ShadePoint_Internal(const vec3& position, const vec3& dir, const Scene* scene, int curDepth, Intersection& intersection)
{
  if (curDepth < 0)
    return vec3(0.0f, 0.0f, 0.0f);

  if (!scene->ClosestIntersection(position, dir, intersection))
    return vec3(0.0f, 0.0f, 0.0f);

  std::shared_ptr<Material> mat = intersection.GetMaterial();
  vec3 light = mat->emissive * glm::vec3(1.0f, 1.0f, 1.0f);

  vec3 indirectLight = vec3(0.0f, 0.0f, 0.0f);
  vec3 triangleNormal = intersection.GetNormal();

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

    if (lightFactor > 0.0f)
    {
      Intersection indIntersection;
      vec3 indColour = ShadePoint_Internal(intersection.position, indir_dir, scene, curDepth - 1, indIntersection);

      if (indIntersection.isValid())
      {
        glm::vec3 brdf = mat->CalculateBRDF(position - intersection.position, glm::normalize(intersection.position - indIntersection.position), indIntersection.GetNormal());

        indirectLight = lightFactor * brdf * indColour * 2.0f;
      }
    }
  }

  light += indirectLight;

  return light;
}

vec3 RaytraceRenderer::DirectLight(const vec3& src_position, const Intersection& intersection, const Scene* scene)
{
  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    vec3 lightDir = light->GetLightDirection(vec3(intersection.position));

    Intersection shadowIntersection;
    if (light->CastsShadows() && scene->ShadowIntersection(intersection.position, lightDir, shadowIntersection))
    {
      return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::vec3 brdf = intersection.GetMaterial()->CalculateBRDF(src_position - intersection.position, glm::normalize(lightDir), intersection.GetNormal());
    colour += brdf * light->CalculateLightAtLocation(intersection.position);
  }

  return colour;
}