#ifndef MATERIAL_H
#define MATERIAL_H

#include "material.h"
#include "../texture/texture.h"
#include "../mesh/mesh.h"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

class Material
{
public:

  Material(const glm::vec3& Albedo, const glm::vec3& Specular = glm::vec3(0.0f, 0.0f, 0.0f), const float Mirror = 0.0f, const float Emissive = 0.0f, float Transparency = 0.0f, const float IOR = 1.0f) :
    albedo(Albedo), specular(Specular), mirror(Mirror), emissive(Emissive), transparency(Transparency), ior(IOR)
  {}

	virtual glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& inNormal, const Vertex& vertexData) = 0;

  //returns a reflection ray according to importance sampling for this material
  glm::vec3 CalculateReflectedRay(const glm::vec3& in_ray, const glm::vec3& normal) const;


  inline glm::vec3 GetAlbedo(glm::vec2 UV) const 
  {
    if (albedoTexture)
    {
      uint8_t bytes[4];
      albedoTexture->SampleTexture(UV, bytes);
      return albedo * glm::vec3(bytes[0], bytes[1], bytes[2]) * (1.0f / 255.0f);
    }

    return albedo;
  }

  glm::vec3 GetModifiedNormal(const glm::vec3& initialNormal, glm::vec2 UV) const;
public:

  glm::vec3 albedo;
  glm::vec3 specular;

  std::shared_ptr<Texture> albedoTexture;
  std::shared_ptr<Texture> normalTexture;

  float mirror;
  float emissive;
  float transparency;
  float ior;

public:

  static glm::vec3 GetRefractionDir(const glm::vec3& in_ray, const glm::vec3& normal, const float ior);

  // returns the ratio of transmitted light at this angle
  static float Fresnel(const glm::vec3& in_ray, const glm::vec3& normal, const float ior);
};

#endif
