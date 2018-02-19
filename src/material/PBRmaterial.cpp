#include "PBRmaterial.h"

#include "../amath.h"

namespace
{
  const float Roughness = 0.0f;
  const float BaseFresnel = 0.95f;
};

using AMath::square;

glm::vec3 PBRMaterial::CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal, const glm::vec3& albedo)
{
  const glm::vec3 view_norm = glm::normalize(view);
  const glm::vec3 h = (view_norm + light) / 2.0f;
  const float A = Distribution(h, normal) * Fresnel(view_norm, h, normal) * Geometry(light, view_norm, normal);
  const float B = 4.0f * glm::dot(normal, light) * glm::dot(normal, view_norm);
  return A * albedo;
}


float PBRMaterial::Distribution(const glm::vec3& h, const glm::vec3& normal)
{
  const float PI = 3.14159265f;
  const float alpha2 = square(square(Roughness));
  const float B = (square(glm::dot(normal, h)) * (alpha2 - 1.0f)) + 1.0f;
  return alpha2 / (PI * square(B));
}

float PBRMaterial::Fresnel(const glm::vec3& view, const glm::vec3& h, const glm::vec3& normal)
{
  const float dot = glm::dot(view, h);
  const float i = ((-5.55473f * dot) - 6.98316f) * dot;
  return BaseFresnel + ((1.0f - BaseFresnel) * pow(2.0f, i));
}

float PBRMaterial::Geometry(const glm::vec3& light, const glm::vec3& view, const glm::vec3& normal)
{
  return Geometry_Internal(light, normal) * Geometry_Internal(view, normal);
}

float PBRMaterial::Geometry_Internal(const glm::vec3& vector, const glm::vec3& normal)
{
  const float k = square(Roughness + 1.0f) / 8.0f;
  const float dot = glm::dot(vector, normal);
  return dot / ((dot * (1.0f - k)) + k);
}
