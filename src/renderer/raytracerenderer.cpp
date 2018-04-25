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
#include <omp.h>
#include <math.h>

#include <glm/gtx/norm.hpp>

#define MAX_BOUNCES 5
#define NUM_DIRS 5000

// if set to 0 then simple shading is used
#define USE_GI 0

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
      const vec3 dir = glm::normalize(vec3(rotationMatrix * vec4(x - sceenWidthHalf, y - screenHeightHalf, focalLength, 1)));
      vec3 colour = ShadePoint(cameraPosition, dir, scene);
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
  Intersection intersection;
  if (!scene->ClosestIntersection(position, dir, intersection))
    return vec3(0.0f, 0.0f, 0.0f);

#if USE_GI
  float r = 0.0f, g = 0.0f, b = 0.0f;

  #pragma omp parallel for reduction(+:r,g,b)
  for (int i = 0; i < NUM_DIRS; ++i)
  {
    vec3 colour = ShadePoint_Internal(dir, scene, MAX_BOUNCES, intersection);

    r = r + colour.x;
    g = g + colour.y;
    b = b + colour.z;
  }

  return glm::vec3(r,g,b) / (float)NUM_DIRS;
#else
  return DirectLight(dir, intersection, scene);
#endif
}

vec3 RaytraceRenderer::ShadePoint_Internal(const vec3& dir, const Scene* scene, int curDepth, const Intersection& intersection)
{
  if (curDepth < 0)
    return vec3(0.0f, 0.0f, 0.0f);

  std::shared_ptr<Material> mat = intersection.GetMaterial();

  if (mat->emissive > 0.0f)
    return mat->emissive * glm::vec3(1.0f, 1.0f, 1.0f);

  vec3 light(0.0f, 0.0f, 0.0f);

  vec3 indirectLight = vec3(0.0f, 0.0f, 0.0f);
  vec3 normal = intersection.GetNormal();

  glm::vec3 iPosition = intersection.vertexData.position;

  {
    const glm::vec3 indir_dir = mat->CalculateReflectedRay(dir, normal);
    const float lightFactor = abs(glm::dot(indir_dir, normal));

    Intersection indIntersection;
    if (scene->ClosestIntersection(iPosition + (indir_dir * 0.001f), indir_dir, indIntersection))
    {
      vec3 indColour = ShadePoint_Internal(indir_dir, scene, curDepth - 1, indIntersection);
      glm::vec3 brdf = mat->CalculateBRDF(dir, glm::normalize(iPosition - indIntersection.vertexData.position), indIntersection.GetNormal(), intersection.vertexData);

      indirectLight = lightFactor * brdf * indColour * 2.0f;
    }
  }

  light += indirectLight;

  return light;
}

vec3 RaytraceRenderer::DirectLight(const vec3& in_ray, const Intersection& intersection, const Scene* scene, int depth /*= 5*/)
{
  std::shared_ptr<Material> mat = intersection.GetMaterial();

  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);
  glm::vec3 reflColour(0.0f, 0.0f, 0.0f);
  glm::vec3 refrColour(0.0f, 0.0f, 0.0f);

  glm::vec3 iPosition = intersection.vertexData.position;
  glm::vec3 normal = intersection.GetNormal();

  float refrFactor = mat->transparency;

  if (refrFactor > 0.0f && depth > 0)
  {
    refrFactor *= Material::Fresnel(in_ray, normal, mat->ior);

    if (refrFactor > 0.0f)
    {
      glm::vec3 refractedDir = Material::GetRefractionDir(in_ray, normal, mat->ior);
      Intersection refrIntersection;
      if (scene->ClosestIntersection(iPosition + (refractedDir * 0.1f), refractedDir, refrIntersection))
      {
        refrColour = DirectLight(refractedDir, refrIntersection, scene, depth - 1);
      }
    }
  }

  const float reflFactor = (1.0f - refrFactor) * mat->mirror;
  const float diffFactor = (1.0f - refrFactor) * (1.0f - mat->mirror);

  if (diffFactor > 0.0f)
  {
    for (const std::shared_ptr<Light> light : *Lights)
    {
      vec3 lightDir = light->GetLightDirection(iPosition);

      Intersection shadowIntersection;
      if (light->CastsShadows() && scene->ShadowIntersection(iPosition, lightDir, shadowIntersection))
      {
        return glm::vec3(0.0f, 0.0f, 0.0f);
      }

      glm::vec3 brdf = mat->CalculateBRDF(in_ray, glm::normalize(lightDir), intersection.GetNormal(), intersection.vertexData);
      colour += brdf * light->CalculateLightAtLocation(iPosition);
    }
  }

  if(reflFactor > 0.0f && depth > 0)
  {
    const glm::vec3 reflectedDir = in_ray - (2.0f * glm::dot(in_ray, normal) * normal);

    Intersection reflIntersection;
    if (scene->ClosestIntersection(iPosition + (reflectedDir * 0.1f), reflectedDir, reflIntersection))
    {
      reflColour = DirectLight(reflectedDir, reflIntersection, scene, depth - 1);
    }
  }

  return (colour * diffFactor) + (reflColour * reflFactor * 0.8f) + (refrColour * refrFactor * 0.8f);
}