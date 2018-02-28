#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "light.h"

class PointLight : public Light
{
public:
  PointLight(const glm::vec3& colour, const float intensity, const bool castsShadows, const glm::vec3& location) :
    Light(colour, intensity, castsShadows), m_Location(location)
  {}

  glm::vec3 GetLightDirection(const glm::vec3& location) const override;

  glm::vec3 CalculateLightAtLocation(const glm::vec3& location) const override;

private:

  glm::vec3 m_Location;
};

#endif
