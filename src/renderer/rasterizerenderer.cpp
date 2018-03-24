#include "rasterizerenderer.h"

#include "../mesh/mesh.h"
#include "../scene/scene.h"
#include "../scene/camera.h"
#include "../amath.h"
#include "../misc.h"
#include "../light/light.h"
#include "../material/material.h"
#include "antialiasing.h"
#include "rendertarget.h"

#include <glm/gtx/transform.hpp>

void RasterizeRenderer::Draw(const Scene* scene)
{
  const mat4 cameraMatrix = glm::translate(glm::transpose(scene->camera->rotationMatrix), -scene->camera->position);
  //float focalLength = screenptr->width / (2.0f * tan(scene->camera->FOV / TWO_PI));
  const mat4 projection = CreatePerspectiveMatrix(scene->camera);

  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    if (light->CastsShadows())
      light->UpdateShadowMap();
  }

  RasterizeScene(scene, screenptr,
    [&](const glm::vec4& point, glm::vec4& outProjectedPoint) {
      return VertexShader(cameraMatrix, projection, point, outProjectedPoint);
    },
    [&](const Scene* scene, std::shared_ptr<Material> mat, const class Triangle& Tri, const struct Vertex& Vertex) {
      return PixelShader(scene, mat, Tri, Vertex);
    });

  for (int y = 0; y < screenptr->height; y++)
  {
    for (int x = 0; x < screenptr->width; x++)
    {
      //vec3 colour = performAntiAliasing(screenptr->floatBuffer, x, y, screenWidth, screenHeight, screenptr->floatBuffer[y*screenptr->width+x]);
      vec3 colour = glm::vec3(screenptr->floatBuffer[y*screenptr->width + x]);
      //vec3 colour = glm::vec3(100.0f, 100.0f, 100.0f) * clamp(screenptr->floatBuffer[y*screenptr->width + x].w, 0.0f, 2.55f);
      screenptr->PutPixel(x, y, colour);
    }
  }
}

#pragma optimize("", off)
template<bool includePixels, typename VertexPred, typename PixelPred>
void RasterizeRenderer::RasterizeScene(const Scene* scene, RenderTarget* target, VertexPred VertexShader, PixelPred PixelShader)
{
  const std::vector<std::shared_ptr<Mesh>>* Meshes = scene->GetMeshes();

  for (const std::shared_ptr<Mesh> mesh : *Meshes)
  {
    size_t V = mesh->Verticies.size();
    size_t T = mesh->Triangles.size();
    std::vector<ProjectedVert> projectedVerts(V);

    for (int i = 0; i < V; ++i)
    {
      projectedVerts[i].depth = VertexShader(glm::vec4(mesh->Verticies[i].position, 1.0f), projectedVerts[i].position);
    }

    for (int i = 0; i < T; ++i)
    {
      std::vector<Triangle> clippedTriangles({ mesh->Triangles[i] });

      std::vector<ProjectedVert> clippedVerticies({
        projectedVerts[clippedTriangles[0].v0],
        projectedVerts[clippedTriangles[0].v1],
        projectedVerts[clippedTriangles[0].v2] });

      std::vector<Vertex> clippedVertexData({
        mesh->Verticies[clippedTriangles[0].v0],
        mesh->Verticies[clippedTriangles[0].v1],
        mesh->Verticies[clippedTriangles[0].v2] });

      // remap the indexes to be relative to the new coordinate list we have constructed
      clippedTriangles[0].v0 = 0;
      clippedTriangles[0].v1 = 1;
      clippedTriangles[0].v2 = 2;

      ClipTriangle(clippedTriangles, clippedVerticies, clippedVertexData);

      for (const Triangle& Tri : clippedTriangles)
      {
        const glm::ivec2 v0 = ConvertHomogeneousCoordinatesToRasterSpace(target, clippedVerticies[Tri.v0].position);
        const glm::ivec2 v1 = ConvertHomogeneousCoordinatesToRasterSpace(target, clippedVerticies[Tri.v1].position);
        const glm::ivec2 v2 = ConvertHomogeneousCoordinatesToRasterSpace(target, clippedVerticies[Tri.v2].position);

        // coordinates of the triangle
        glm::ivec2 triMin, triMax;
        triMin.x = std::max(std::min(std::min(v0.x, v1.x), v2.x), 0);
        triMin.y = std::max(std::min(std::min(v0.y, v1.y), v2.y), 0);
        triMax.x = std::min(std::max(std::max(v0.x, v1.x), v2.x), target->width - 1);
        triMax.y = std::min(std::max(std::max(v0.y, v1.y), v2.y), target->height - 1);

        auto edgeFunction = [](const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& p)
        {
          return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
        };

        const float area = edgeFunction(glm::vec2(v1), glm::vec2(v0), glm::vec2(v2));

        if (AMath::isNearlyZero(area))
          continue;

        for (int y = triMin.y; y <= triMax.y; ++y)
        {
          const glm::vec2 p(triMin.x - 1.0f, y);

          // from : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-practical-implementation
          // the step of w is constant over the x loop so it only needs to be evaluated once at the start of the loop
          float w0_cur = edgeFunction(glm::vec2(v2), glm::vec2(v1), p);
          const float w0_step = v1.y - v2.y;

          float w1_cur = edgeFunction(glm::vec2(v0), glm::vec2(v2), p);
          const float w1_step = v2.y - v0.y;

          float w2_cur = edgeFunction(glm::vec2(v1), glm::vec2(v0), p);
          const float w2_step = v0.y - v1.y;

          bool wasValid = false;

          for (int x = triMin.x; x <= triMax.x; ++x)
          {
            // increment our W values
            w0_cur += w0_step;
            w1_cur += w1_step;
            w2_cur += w2_step;

            //check if this point is within the triangle
            if (w0_cur < 0.0f || w1_cur < 0.0f || w2_cur < 0.0f)
            {
              // if we have already drawn the triangle on this line then it is impossible for us to draw it again, so break in that case
              if (!wasValid)
                continue;
              else
                break;
            }

            wasValid = true;

            const float w0 = w0_cur / area;
            const float w1 = w1_cur / area;
            const float w2 = w2_cur / area;

            const float invdepth0 = 1.0f / clippedVerticies[Tri.v0].depth;
            const float invdepth1 = 1.0f / clippedVerticies[Tri.v1].depth;
            const float invdepth2 = 1.0f / clippedVerticies[Tri.v2].depth;

            const float depth = (invdepth0 * w0) + (invdepth1 * w1) + (invdepth2 * w2);

            if (target->GetDepth(x, y) < depth)
            {
              target->PutDepth(x, y, depth);

              if (includePixels) 
              {
                Vertex Vert = (clippedVertexData[Tri.v0] * w0 * invdepth0) + (clippedVertexData[Tri.v1] * w1 * invdepth1) + (clippedVertexData[Tri.v2] * w2 * invdepth2);
                Vert *= 1.0f / depth;

                target->PutFloatPixel(x, y, PixelShader(scene, mesh->GetMaterial(i), Tri, Vert));
              }
            }
          }
        }
      }
    }
  }
}
#pragma optimize("", on)


float RasterizeRenderer::VertexShader(const glm::mat4& view, const glm::mat4& projection, const glm::vec4& v, glm::vec4& outProjPos) const
{
  glm::vec4 transformedV = view * v;

  outProjPos = transformedV * projection;
  return transformedV.z;
}

glm::vec3 RasterizeRenderer::PixelShader(const Scene* scene, std::shared_ptr<Material> material, const Triangle& Tri, const Vertex& Vertex) const
{
  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    if(light->CastsShadows() && light->EvaluateShadowMap(Vertex.position))
      continue;

    glm::vec3 brdf = material->CalculateBRDF(scene->camera->position - Vertex.position, glm::normalize(light->GetLightDirection(Vertex.position)), Tri.normal);
    colour += brdf * light->CalculateLightAtLocation(Vertex.position);
  }

  return colour;
}

void RasterizeRenderer::ClipTriangle(std::vector<Triangle>& inoutTriangles, std::vector<ProjectedVert>& inoutVertexPositions, std::vector<Vertex>& inoutVertexData)
{
  //ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::W);
  ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::X);
  //ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::Y);
  //ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::Z);
}

void RasterizeRenderer::ClipTriangleOnAxis(std::vector<Triangle>& inoutTriangles, std::vector<ProjectedVert>& inoutVertexPositions, std::vector<Vertex>& inoutVertexData, Axis axis)
{
  const float W_CLIPPING_PLANE = 0.001f;
  const int i_axis = (int)axis;

  // count backwards so that we only process each triangle once even if they are removed from the list during iteration
  for (int tidx = inoutTriangles.size() - 1; tidx >= 0; --tidx)
  {
    Triangle& Tri = inoutTriangles[tidx];

    // true if the vertex is within the viewing frustum
    const bool dot[3] = {
      inoutVertexPositions[Tri.v0].position[i_axis] > inoutVertexPositions[Tri.v0].position.w,
      inoutVertexPositions[Tri.v1].position[i_axis] > inoutVertexPositions[Tri.v1].position.w,
      inoutVertexPositions[Tri.v2].position[i_axis] > inoutVertexPositions[Tri.v2].position.w
    };

    const int numValidVerts = (int)dot[0] + (int)dot[1] + (int)dot[2];
    //no vertices are within the viewing area - discard the triangle
    if (numValidVerts == 0)
    {
      Misc::RemoveSwap(inoutTriangles, tidx);
      continue;
    }
    else if (numValidVerts == 3)
    {
      continue;
    }

    // deal with the case where there are 2 out of bounds values first as this can be done without building a new triangle
    if (numValidVerts == 1)
    {
      const int validVertIdx = dot[0] ? Tri.v0 : (dot[1] ? Tri.v1 : Tri.v2);

      for (int vidx = 0; vidx < 3; ++vidx)
      {
        // no need to interpolate the valid vert
        if(vidx == validVertIdx) continue;

        ClipLine(inoutVertexPositions[validVertIdx], inoutVertexData[validVertIdx], inoutVertexPositions[vidx], inoutVertexData[vidx], axis);
      }

      continue;
    }

    //finally, if there is only one out of bounds vertex we must build a new triangle with the two new values
    int invalidVert = !dot[0] ? Tri.v0 : (!dot[1] ? Tri.v1 : Tri.v2);
    int validVert0 = dot[0] ? Tri.v0 : Tri.v1;
    int validVert1 = dot[2] ? Tri.v2 : Tri.v1;

    // add the new triangle
    int newVert = inoutVertexPositions.size();
    inoutVertexPositions.push_back(inoutVertexPositions[invalidVert]);
    inoutVertexData.push_back(inoutVertexData[invalidVert]);

    Triangle newTriangle(validVert1, invalidVert, newVert); // winding order doesn't matter as we set the normal explicitly

    if (invalidVert != Tri.v1)
    {
      newTriangle.v0 = invalidVert;
      newTriangle.v1 = validVert1;
    }

    newTriangle.normal = Tri.normal;
    inoutTriangles.push_back(newTriangle);

    ClipLine(inoutVertexPositions[validVert0], inoutVertexData[validVert0], inoutVertexPositions[invalidVert], inoutVertexData[invalidVert], axis);
    ClipLine(inoutVertexPositions[validVert1], inoutVertexData[validVert1], inoutVertexPositions[newVert], inoutVertexData[newVert], axis);
  }
}

void RasterizeRenderer::ClipLine(const ProjectedVert& v0Pos, const Vertex& v0Data, ProjectedVert& v1Pos, Vertex& v1Data, Axis axis)
{
  const float diffValid = v0Pos.position.w - v0Pos.position[(int)axis];
  // 1. work out the interpolation amount
  const float diff = v1Pos.position.w - v1Pos.position[(int)axis];
  const float intersectionFactor = diffValid /
    (diffValid - diff);

  // 2. work out the new position
  ProjectedVert newPosition = v1Pos;
  newPosition -= v0Pos;
  newPosition *= intersectionFactor;
  newPosition += v0Pos;
  v1Pos = newPosition;

  // 3. work out the new vertex attributes
  Vertex newData = v1Data;
  newData -= v0Data;
  newData *= intersectionFactor;
  newData += v0Data;
  v1Data = newData;
}

glm::ivec2 RasterizeRenderer::ConvertHomogeneousCoordinatesToRasterSpace(RenderTarget* target, const glm::vec4& homogeneousCoordinates)
{
  glm::vec2 NDCposition(homogeneousCoordinates.x / homogeneousCoordinates.w, homogeneousCoordinates.y / homogeneousCoordinates.w);
  NDCposition += 1.0f;
  NDCposition *= 0.5f;
  NDCposition = 1.0f - NDCposition;
  NDCposition *= glm::vec2(target->width - 1.0f, target->height - 1.0f);

  return glm::ivec2((int)NDCposition.x, (int)NDCposition.y);
}

glm::mat4 RasterizeRenderer::CreatePerspectiveMatrix(const Camera* camera) const
{
  return glm::perspective(AMath::ToRads(camera->FOV * 2.0f), 1.0f, camera->nearClipPlane, camera->farClipPlane);
}

void RasterizeRenderer::DrawLine(const glm::ivec2& a, const glm::ivec2& b, const glm::vec3 colour)
{
  vec2 maxValue = b - a;
  int maxSize = std::max(std::abs(maxValue.x), std::abs(maxValue.y));
  std::vector<ivec2> points(maxSize);

  AMath::interpolate(a, b, points);

  for (int i = 0; i < points.size(); ++i) {
    screenptr->PutPixel(points[i].x, points[i].y, colour);
  }
}
