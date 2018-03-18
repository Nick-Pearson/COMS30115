#include "light.h"

void Light::UpdateShadowMap()
{
  if (!m_ShadowMap)
    InitialiseShadowMap();

  //TODO: Call the main rasterise function with our projection function as a parameter
}

bool Light::EvaluateShadowMap(const glm::vec3& queryPoint)
{
  if (!m_ShadowMap) return false;
  return false;
}
