#include "raytracerenderer.h"

void RaytraceRenderer::Draw(const std::vector<TestTriangle>& mesh)
{
  float focalLength = 200;
  vec4 cameraPos = vec4(0, 0, -2.5, 1);




  for (int y = 0; y < screenptr->height; y++) {
    for (int x = 0; x < screenptr->width; x++) {
      vec4 dir = vec4(x - (screenptr->width / 2), y - (screenptr->height / 2), focalLength, 1);

      Intersection closestInteraction;

      if (ClosestIntersection(cameraPos, dir, mesh, closestInteraction)) {
        vec3 lightColour = DirectLight(closestInteraction.position, mesh[closestInteraction.triangleIndex]) ;
        vec3 colour = lightColour * mesh[closestInteraction.triangleIndex].color;
        PutPixelSDL(screenptr, x, y, colour);
        // PutPixelSDL(screenptr, x, y, mesh[closestInteraction.triangleIndex].color);
      }
    }
  }
}

vec3 RaytraceRenderer::DirectLight(const vec4& intersectionPosition, const TestTriangle& triangle) {
  vec4 lightPos( 0, -0.5, -0.7, 1.0 );
  vec3 lightColor = 14.f * vec3( 1, 1, 1 );


  vec3 rHat = vec3(lightPos.x-intersectionPosition.x, lightPos.y-intersectionPosition.y, lightPos.z-intersectionPosition.z);
  float d = glm::length(rHat);
  rHat = glm::normalize(rHat);

  float a = 4 * 3.14 * d * d;

  float rAndN = glm::dot(triangle.normal, rHat);

  if (rAndN < 0) {
    rAndN = 0;
  }

  return rAndN * lightColor / a;
}


bool RaytraceRenderer::ClosestIntersection(vec4 start, vec4 dir, const std::vector<TestTriangle>& triangles, Intersection& closestInteraction)
{
  closestInteraction.distance = std::numeric_limits<float>::max();
  closestInteraction.triangleIndex = -1;
  closestInteraction.position = vec4(0, 0, 0, 0);

  for (int i = 0; i < triangles.size(); i++) {
    TestTriangle triangle = triangles[i];

    vec3 v0 = triangle.v0;
    vec3 v1 = triangle.v1;
    vec3 v2 = triangle.v2;

    vec3 e1 = vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
    vec3 e2 = vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
    vec3 b = vec3(start.x-v0.x,start.y-v0.y,start.z-v0.z);

    mat3 A( -vec3(dir[0], dir[1], dir[2]), e1, e2 );
    vec3 x = glm::inverse( A ) * b;

    float t = x[0];
    float u = x[1];
    float v = x[2];

    // TODO: Dot product optimisation

    if (t >= 0 && u >= 0 && v >= 0 && u + v <= 1 && t < closestInteraction.distance) {
      closestInteraction.distance = t;
      closestInteraction.position = start + t * dir;
      closestInteraction.triangleIndex = i;
    }
  }

  return closestInteraction.triangleIndex != -1;

}
