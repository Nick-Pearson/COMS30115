#include "raytracerenderer.h"

#include <glm/gtx/norm.hpp>

void RaytraceRenderer::Draw(const std::vector<TestTriangle>& mesh)
{
  float focalLength = 500;
  vec4 cameraPos = vec4(0, 0, -2.4, 1);

  for (int y = 0; y < screenptr->height; y++)
  {
    for (int x = 0; x < screenptr->width; x++)
    {
      vec4 dir = vec4(x - (screenptr->width / 2), y - (screenptr->height / 2), focalLength, 1);

      Intersection closestIntersection;

      if (ClosestIntersection(cameraPos, dir, mesh, closestIntersection))
      {
        vec3 lightColour = DirectLight(closestIntersection, mesh);
        vec3 colour = lightColour * mesh[closestIntersection.triangleIndex].color;
        PutPixelSDL(screenptr, x, y, colour);
      }
    }
  }
}

vec3 RaytraceRenderer::DirectLight(const Intersection& intersection, const std::vector<TestTriangle>& mesh)
{
  const TestTriangle& triangle = mesh[intersection.triangleIndex];

  vec3 lightPos( 0, -0.5, -0.7 );
  vec3 lightColor = 14.f * vec3( 1, 1, 1 );

  {
    Intersection shadowIntersection;
    vec3 shadowDir = lightPos - vec3(intersection.position);
    if(ClosestIntersection(intersection.position, vec4(shadowDir, 1.0f), mesh, shadowIntersection, false))
    {
      if(shadowIntersection.distance < 1.0f)
        return vec3(0, 0, 0);
    }
  }

  vec3 rHat = lightPos - vec3(intersection.position);
  float d_sqrd = glm::length2(rHat);
  rHat = glm::normalize(rHat);

  float a = 4 * 3.14 * d_sqrd;

  float rAndN = glm::dot(triangle.normal, rHat);

  if (rAndN < 0)
    return vec3(0,0,0);

  return rAndN * lightColor / a;
}


bool RaytraceRenderer::ClosestIntersection(vec4 start, vec4 dir, const std::vector<TestTriangle>& triangles, Intersection& closestIntersection, bool allowZeroDist /*= true*/)
{
  closestIntersection.distance = std::numeric_limits<float>::max();
  closestIntersection.triangleIndex = -1;
  closestIntersection.position = vec4(0, 0, 0, 0);

  for (size_t i = 0; i < triangles.size(); i++)
  {
    TestTriangle triangle = triangles[i];

    vec3 v0 = triangle.v0;
    vec3 v1 = triangle.v1;
    vec3 v2 = triangle.v2;

    vec3 e1 = vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
    vec3 e2 = vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);

    // Dot product optimisation
    vec3 rayLine = vec3(dir.x - start.x, dir.y - start.y, dir.z - start.z);
    vec3 triangleNormal = glm::cross(e1, e2);
    if (glm::dot(triangleNormal, rayLine) == 1)
    {
      continue;
    }

    vec3 b = vec3(start.x-v0.x, start.y-v0.y, start.z-v0.z);

    mat3 A( -vec3(dir[0], dir[1], dir[2]), e1, e2 );
    vec3 x = glm::inverse( A ) * b;

    float t = x[0];
    float u = x[1];
    float v = x[2];

    if (t >= 0 && u >= 0 && v >= 0 && u + v <= 1 && t < closestIntersection.distance && (allowZeroDist || t > 0.01f))
    {
      closestIntersection.distance = t;
      closestIntersection.position = start + (t * dir);
      closestIntersection.triangleIndex = i;
    }
  }

  return closestIntersection.triangleIndex != -1;
}
