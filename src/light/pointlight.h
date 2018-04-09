#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "light.h"

class PointLight : public Light
{
public:
  PointLight(const glm::vec3& colour, const float intensity, const bool castsShadows, const glm::vec3& location) :
    Light(colour, intensity, castsShadows), m_Location(location)
  {}

  ~PointLight();

  glm::vec3 GetLightDirection(const glm::vec3& location) const override;
  glm::vec3 GetRandomLightDirection(const glm::vec3& location) const override;

  glm::vec3 CalculateLightAtLocation(const glm::vec3& location) const override;

protected:

  void UpdateShadowMap(const class Scene* scene) override;

  bool EvaluateShadowMap(const glm::vec3& queryPoint) const override;

private:

  class TextureCubemap* shadowMap = nullptr;

  glm::vec3 m_Location;

  float lightRadius = 0.2f;

};

#endif
