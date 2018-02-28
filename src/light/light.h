#ifndef LIGHT_H
#define LIGHT_H

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

protected:

  glm::vec3 m_Colour;
  float m_Intensity = 0.0f;
  bool m_CastsShadows = false;
};

#endif // LIGHT_H
