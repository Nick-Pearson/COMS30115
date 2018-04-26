#include "pointlight.h"

#include "../amath.h"
#include "../texture/texture.h"
#include "../texture/texturecubemap.h"
#include "../scene/scene.h"
#include "../scene/camera.h"

#include "../renderer/rasterizerenderer.h"
#include "../renderer/rasterizerenderer.inl"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

PointLight::~PointLight()
{
  delete shadowMap;
}

glm::vec3 PointLight::GetLightDirection(const glm::vec3& location) const
{
  return m_Location - location;
}


glm::vec3 PointLight::GetRandomLightDirection(const glm::vec3& location) const
{
  #define RANDCOMP ((2.0f * (float)rand() * lightRadius / (float)RAND_MAX) - lightRadius)
  glm::vec3 randomLoc = m_Location + glm::vec3(RANDCOMP, RANDCOMP, RANDCOMP);
  #undef RANDCOMP

  return randomLoc - location;
}

glm::vec3 PointLight::CalculateLightAtLocation(const glm::vec3& location) const
{
  const glm::vec3 rHat = GetLightDirection(location);

  const float d_sqrd = glm::length2(rHat);
  const float a = 4.0f * (float)PI * d_sqrd;

  return m_Intensity * m_Colour / a;
}

void PointLight::UpdateShadowMap(const Scene* scene)
{
  if(!shadowMap)
  {
    const int shadowMapResolution = 1024;
    shadowMap = new TextureCubemap(shadowMapResolution, shadowMapResolution, 4);

    //create a camera at the correct location with the correct FOV
    Camera c(78.69f, m_Location);

    for(int i = 0; i < (int)CubemapDirections::MAX; ++i)
    {
      //rotate to facing
      glm::mat4 rotationMatrix;

      switch ((CubemapDirections)i)
      {
      case CubemapDirections::Top:
        rotationMatrix = glm::rotate(rotationMatrix, 90.0f, vec3(1.0f, 0.0f, 0.0f));
        break;
      case CubemapDirections::Bottom:
        rotationMatrix = glm::rotate(rotationMatrix, 90.0f, vec3(-1.0f, 0.0f, 0.0f));
        break;
      case CubemapDirections::Back:
        rotationMatrix = glm::rotate(rotationMatrix, 180.0f, vec3(0.0f, 1.0f, 0.0f));
        break;
      case CubemapDirections::Left:
        rotationMatrix = glm::rotate(rotationMatrix, 90.0f, vec3(0.0f, 1.0f, 0.0f));
        break;
      case CubemapDirections::Right:
        rotationMatrix = glm::rotate(rotationMatrix, 90.0f, vec3(0.0f, -1.0f, 0.0f));
        break;
      }

      c.rotationMatrix = rotationMatrix;

      //render the scene depth
      DepthTextureRenderTarget rt(shadowMap->Images[i]);
      RasterizeRenderer::RasterizeScene<false>(scene, &c, &rt);

      // export the textures to BMP to debug
      //rt.SaveImage(("./dm_" + std::to_string(i) + ".bmp").c_str(), true);
    }
  }
}

bool PointLight::EvaluateShadowMap(const glm::vec3& queryPoint) const
{
  if(!shadowMap) return false;

  const glm::vec3 dir = queryPoint - m_Location;

  union {
    float val;
    uint8_t bytes[4];
  } u;

  float depth = 0.0f;
  shadowMap->GetCubemapColourWithDepth(dir, u.bytes, &depth);

  depth = 1.0f / depth;

  // add a small amount to account for floating point inaccuracies so we dont shadow ourselves
  return (depth+0.01f) < u.val;
}
