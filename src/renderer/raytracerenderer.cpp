#include "raytracerenderer.h"

#include "../scene/camera.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_access.hpp>

void RaytraceRenderer::Draw(const std::vector<TestTriangle>& mesh)
{
  float focalLength = camera->focalLength;
  vec4 cameraPos = vec4(camera->position, 1);

  for (int y = 0; y < screenptr->height; y++)
  {
    for (int x = 0; x < screenptr->width; x++)
    {
      vec4 dir = camera->rotationMatrix * vec4(x - (screenptr->width / 2), y - (screenptr->height / 2), focalLength, 1);

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
    if(ClosestIntersection(intersection.position, vec4(shadowDir, 1.0f), mesh, shadowIntersection))
    {
      if(shadowIntersection.distance < 1.0f)
        return vec3(0, 0, 0);
    }
  }

  vec3 rHat = lightPos - vec3(intersection.position);
  float d_sqrd = glm::length2(rHat);
  rHat = glm::normalize(rHat);

  float a = 4.0f * 3.14f * d_sqrd;

  float rAndN = glm::dot(triangle.normal, rHat);

  if (rAndN < 0)
    return vec3(0,0,0);

  return rAndN * lightColor / a;
}


bool RaytraceRenderer::ClosestIntersection(vec4 start, vec4 dir, const std::vector<TestTriangle>& triangles, Intersection& closestIntersection)
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
    if (glm::dot(triangle.normal, vec3(dir)) >= 0.0f)
    {
      continue;
    }

    vec3 b = vec3(start.x-v0.x, start.y-v0.y, start.z-v0.z);

    mat3 A( -vec3(dir[0], dir[1], dir[2]), e1, e2 );
    const float detA = glm::determinant(A);

    // solve t first and check if it is valid
    float t = glm::determinant(glm::column(A, 0, b)) / detA;

    if(t < 0 || t > closestIntersection.distance)
      continue;

    float u = glm::determinant(glm::column(A, 1, b)) / detA;
    float v = glm::determinant(glm::column(A, 2, b)) / detA;

    if (u >= 0 && v >= 0 && u + v <= 1)
    {
      closestIntersection.distance = t;
      closestIntersection.position = start + (t * dir);
      closestIntersection.triangleIndex = i;
    }
  }

  return closestIntersection.triangleIndex != -1;
}
