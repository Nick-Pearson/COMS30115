#ifndef LIGHT_H
#define LIGHT_H

#include <memory>

#include <glm/glm.hpp>

class Light
{
public:
  Light(const glm::vec3& colour, const float intensity, const bool castsShadows) :
    m_Colour(colour), m_Intensity(intensity), m_CastsShadows(castsShadows)
  {}

  virtual glm::vec3 GetLightDirection(const glm::vec3& location) const = 0;
  virtual glm::vec3 CalculateLightAtLocation(const glm::vec3& location) const = 0;

  inline bool CastsShadows() const { return m_CastsShadows; }

  virtual void UpdateShadowMap(const class Scene* scene) = 0;

  /** Project this point and evaluate it against the shadow map to determine if it is in shadow
   * @return true if the point should be in shadow
   */
  virtual bool EvaluateShadowMap(const glm::vec3& queryPoint) const = 0;

protected:

  glm::vec3 m_Colour;
  float m_Intensity = 0.0f;
  bool m_CastsShadows = false;
};

#endif // LIGHT_H
