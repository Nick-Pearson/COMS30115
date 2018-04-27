#include "material.h"
#include "../amath.h"

#include <random>
#include <algorithm>

#include <glm/gtx/transform.hpp>

glm::vec3 Material::CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& inNormal, const glm::vec3& tangent, const glm::vec3& bitangent, const Vertex& vertexData) const
{
  const glm::vec3 normal = GetModifiedNormal(inNormal, tangent, bitangent, vertexData.uv0);
  const glm::vec3 colour = std::max(0.f, glm::dot(normal, light)) * GetAlbedo(vertexData.uv0);

  // early out for non specular materials
  if (specular == 0.0f)
    return colour;

  // reflected direction L - 2(N . LN)
  const glm::vec3 idealReflectionRay = glm::normalize(light - (2.0f * glm::dot(light, normal) * normal));
  float spec = std::pow(std::max(0.f, glm::dot(idealReflectionRay, view)), specularExponent) * specular;
  return colour + (spec  * glm::vec3(1.0f, 1.0f, 1.0f));
}


void Material::CalculateReflectedRay(const glm::vec3& in_ray, const glm::vec3& inNormal, const glm::vec3& tangent, const glm::vec3& bitangent, const Vertex& vertexData, glm::vec3& outRay, bool& outIsRefractionRay, float& outImportance) const
{
  glm::vec3 normal = GetModifiedNormal(inNormal, tangent, bitangent, vertexData.uv0);

  outIsRefractionRay = false;
  outImportance = 1.0f;

  float refrFactor = transparency;

  if (refrFactor > 0.0f)
  {
    refrFactor *= Material::Fresnel(in_ray, normal, ior);
  }

  //randomly select between a direct mirror and a diffuse ray
  bool useRefractionRay = ((float)rand() / (float)RAND_MAX) < refrFactor;

  if (useRefractionRay)
  {
    outRay = Material::GetRefractionDir(in_ray, normal, ior);
    outIsRefractionRay = true;
    return;
  }

  const float reflFactor = (1.0f - refrFactor) * mirror;

  bool useReflectionRay = ((float)rand() / (float)RAND_MAX) < mirror;

  if(useReflectionRay)
  {
    outRay = glm::normalize(in_ray - (2.0f * glm::dot(in_ray, normal) * normal));
    return;
  }

  // add a random direction of indirect light
  const float theta1 = (float)rand() * 3.14f / (float)RAND_MAX;
  const float theta2 = (float)rand() * 3.14f / (float)RAND_MAX;
  const float theta3 = (float)rand() * 3.14f / (float)RAND_MAX;

  glm::mat4 rotationMatrix;
  rotationMatrix = glm::rotate(rotationMatrix, theta1, glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, theta2, glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, theta3, glm::vec3(0.0f, 0.0f, 1.0f));

  outRay = glm::vec3(glm::vec4(normal, 1.0f) * rotationMatrix);

  glm::vec3 idealReflectionRay = glm::normalize(outRay - (2.0f * glm::dot(outRay, normal) * normal));
  outImportance = std::pow(std::max(0.f, glm::dot(idealReflectionRay, in_ray)), specularExponent) * specular;
}

glm::vec3 Material::GetModifiedNormal(const glm::vec3& initialNormal, const glm::vec3& tangent, const glm::vec3& bitangent, glm::vec2 UV) const
{
  if (!normalTexture)
    return initialNormal;

  uint8_t bytes[4];
  normalTexture->SampleTexture(UV, bytes);

  //values from -1.0 to 1.0 in each component
  glm::vec3 texNormal = (glm::vec3(bytes[0], bytes[1], bytes[2]) * (1.0f / 128.0f)) - 1.0f;

  glm::mat3 TBN(tangent, bitangent, initialNormal);

  return glm::normalize(TBN * texNormal);
}


glm::vec3 Material::GetRefractionDir(const glm::vec3& in_ray, const glm::vec3& normal, const float ior)
{
  float cosi = clamp(-1.0f, 1.0f, glm::dot(in_ray, normal));
  float etai = 1.0f;
  float etat = ior;

  glm::vec3 adjustedNormal = normal;

  if (cosi < 0)
  {
    cosi = -cosi;
  }
  else
  {
    std::swap(etai, etat);
    adjustedNormal = -normal;
  }

  float eta = etai / etat;
  float k = 1.0f - (AMath::square(eta) * (1.0f - AMath::square(cosi)));

  return  glm::normalize(eta * in_ray + (eta * cosi - sqrtf(k)) * adjustedNormal);
}

float Material::Fresnel(const glm::vec3& in_ray, const glm::vec3& normal, const float ior)
{
  float cosi = clamp(-1.0f, 1.0f, glm::dot(in_ray, normal));
  float etai = 1.0f;
  float etat = ior;

  if (cosi > 0.0f)
  {
    std::swap(etai, etat);
  }

  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1.0f - AMath::square(cosi)));
  // Total internal reflection
  if (sint >= 1.0f)
  {
    return 0.0f;
  }

  float cost = sqrtf(std::max(0.f, 1.0f - sint * sint));
  cosi = fabsf(cosi);
  float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
  float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
  return 1.0f - ((Rs * Rs + Rp * Rp) / 2.0f);
}
