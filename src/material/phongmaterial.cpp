#include "phongmaterial.h"

#include <algorithm>

glm::vec3 PhongMaterial::CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& inNormal, const Vertex& vertexData)
{
  //glm::vec3 normal = inNormal;
  glm::vec3 normal = GetModifiedNormal(inNormal, vertexData.uv0);

  // reflected direction 2(N . L)N - L
	glm::vec3 R = glm::normalize(light - (2.0f * glm::dot(light, normal) * normal));

	glm::vec3 spec = std::pow(std::max(0.f, glm::dot(R, view)), specularExponent) * specular;
	return (std::max(0.f, glm::dot(normal, light)) * GetAlbedo(vertexData.uv0)) +
		spec;
}
