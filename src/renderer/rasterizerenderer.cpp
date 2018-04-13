#include "rasterizerenderer.h"

#include "rasterizerenderer.inl"

void RasterizeRenderer::Draw(const Scene* scene)
{
  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    if (light->CastsShadows())
      light->UpdateShadowMap(scene);
  }

#if 1 // clear the screen to white to help debugging
  for (int y = 0; y < screenptr->height; y++)
  {
    for (int x = 0; x < screenptr->width; x++)
    {
      screenptr->PutFloatPixel(x,y,glm::vec3(1.0f, 1.0f, 1.0f));
    }
  }
#endif


  // Early Z test optimisation
  RasterizeScene<false>(scene, scene->camera, screenptr);

  RasterizeScene(scene, scene->camera, screenptr);

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


float RasterizeRenderer::VertexShader(const glm::mat4& view, const glm::mat4& projection, const glm::vec4& v, glm::vec4& outProjPos)
{
  glm::vec4 transformedV = view * v;

  outProjPos = transformedV * projection;
  return transformedV.z;
}

glm::vec3 RasterizeRenderer::PixelShader(const Scene* scene, std::shared_ptr<Material> material, const Triangle& Tri, const Vertex& Vertex)
{
  const std::vector<std::shared_ptr<Light>>* Lights = scene->GetLights();
  glm::vec3 colour(0.0f, 0.0f, 0.0f);

  for (const std::shared_ptr<Light> light : *Lights)
  {
    float shadowMultiplier = 1.0f;
    if(light->CastsShadows() && light->EvaluateShadowMap(Vertex.position))
      shadowMultiplier = 0.2f;

    glm::vec3 brdf = material->CalculateBRDF(scene->camera->position - Vertex.position, glm::normalize(light->GetLightDirection(Vertex.position)), Tri.normal);
    colour += shadowMultiplier * brdf * light->CalculateLightAtLocation(Vertex.position);
  }

  return colour;
}

// the Windows c++ optimizer somehow breaks this function, having optimisation disabled fixes it
#pragma optimize("", off)
void RasterizeRenderer::ClipTriangle(std::vector<Triangle>& inoutTriangles, std::vector<ProjectedVert>& inoutVertexPositions, std::vector<Vertex>& inoutVertexData)
{
  ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::W);
  ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::X);
  //ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::Y);
  //ClipTriangleOnAxis(inoutTriangles, inoutVertexPositions, inoutVertexData, Axis::Z);
}
#pragma optimize("", on)

void RasterizeRenderer::ClipTriangleOnAxis(std::vector<Triangle>& inoutTriangles, std::vector<ProjectedVert>& inoutVertexPositions, std::vector<Vertex>& inoutVertexData, Axis axis)
{
  const float W_CLIPPING_PLANE = 0.001f;
  const int i_axis = (int)axis;

  for (int sign = 1; sign >= -1; sign -= 2)
  {
    // count backwards so that we only process each triangle once even if they are removed from the list during iteration
    for (int tidx = inoutTriangles.size() - 1; tidx >= 0; --tidx)
    {
      const Triangle& Tri = inoutTriangles[tidx];

      // true if the vertex is within the viewing frustum
      bool dot[3];

      if (axis == Axis::W)
      {
        dot[0] = inoutVertexPositions[Tri.v0].position.w < W_CLIPPING_PLANE;
        dot[1] = inoutVertexPositions[Tri.v1].position.w < W_CLIPPING_PLANE;
        dot[2] = inoutVertexPositions[Tri.v2].position.w < W_CLIPPING_PLANE;
      }
      else
      {
        dot[0] = sign * inoutVertexPositions[Tri.v0].position[i_axis] >= inoutVertexPositions[Tri.v0].position.w;
        dot[1] = sign * inoutVertexPositions[Tri.v1].position[i_axis] >= inoutVertexPositions[Tri.v1].position.w;
        dot[2] = sign * inoutVertexPositions[Tri.v2].position[i_axis] >= inoutVertexPositions[Tri.v2].position.w;
      }

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
          if (vidx == validVertIdx) continue;

          ClipLine(inoutVertexPositions[validVertIdx], inoutVertexData[validVertIdx], inoutVertexPositions[vidx], inoutVertexData[vidx], axis, sign);
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

      Triangle newTriangle(validVert1, invalidVert, newVert);

      if (invalidVert != Tri.v1)
      {
        newTriangle.v0 = invalidVert;
        newTriangle.v1 = validVert1;
      }

      newTriangle.normal = Tri.normal;
      inoutTriangles.push_back(newTriangle);

      ClipLine(inoutVertexPositions[validVert0], inoutVertexData[validVert0], inoutVertexPositions[invalidVert], inoutVertexData[invalidVert], axis, sign);
      ClipLine(inoutVertexPositions[validVert1], inoutVertexData[validVert1], inoutVertexPositions[newVert], inoutVertexData[newVert], axis, sign);
    }

    if (axis == Axis::W)
      break;
  }
}

void RasterizeRenderer::ClipLine(const ProjectedVert& v0Pos, const Vertex& v0Data, ProjectedVert& v1Pos, Vertex& v1Data, Axis axis, int sign)
{
  const float W_CLIPPING_PLANE = 0.001f;
  float intersectionFactor;

  // 1. work out the interpolation amount
  if (axis == Axis::W)
  {
    //intersectionFactor = (W_CLIPPING_PLANE - (*previousVertice)[W]) / ((*previousVertice)[W] - (*currentVertice)[W]);
    intersectionFactor = (W_CLIPPING_PLANE - v0Pos.position.w) / (v0Pos.position.w - v1Pos.position.w);
  }
  else
  {
    const float diffValid = v0Pos.position.w - sign*v0Pos.position[(int)axis];
    const float diff = v1Pos.position.w - sign*v1Pos.position[(int)axis];

    // intersectionFactor =
    //  ((*previousVertice)[W] - (*previousVertice)[AXIS]) /
    //  (((*previousVertice)[W] - (*previousVertice)[AXIS]) - ((*currentVertice)[W] - (*currentVertice)[AXIS]));
    intersectionFactor = diffValid /
      (diffValid - diff);
  }

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


glm::mat4 RasterizeRenderer::CreatePerspectiveMatrix(const Camera* camera)
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
