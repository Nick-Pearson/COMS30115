#include "pointlight.h"

#include "../amath.h"
#include "../texture/texture.h"

#include <glm/gtx/norm.hpp>

glm::vec3 PointLight::GetLightDirection(const glm::vec3& location) const
{
  return m_Location - location;
}

glm::vec3 PointLight::CalculateLightAtLocation(const glm::vec3& location) const
{
  const glm::vec3 rHat = GetLightDirection(location);

  const float d_sqrd = glm::length2(rHat);
  const float a = 4.0f * (float)PI * d_sqrd;

  return m_Intensity * m_Colour / a;
}

void PointLight::InitialiseShadowMap()
{
  const int shadowResolution = 512;
  m_ShadowMap = std::shared_ptr<Texture>(new Texture(shadowResolution * 2, shadowResolution, 1));
}

void PointLight::ProjectPointToShadowMap(const glm::vec4& point, glm::ivec2& outProjectedPoint) const
{
  outProjectedPoint = glm::ivec2(-1, -1);
}
