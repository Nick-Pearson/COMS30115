#include "phongmaterial.h"

#include <algorithm>

glm::vec3 PhongMaterial::CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal, const glm::vec3& albedo)
{
	// reflected direction 2(N . L)N - L
	glm::vec3 R = (2.0f * glm::dot(light, normal) * normal) - light;

	return (std::max(0.f, glm::dot(normal, light)) * kD * albedo) + 
		(std::pow(std::max(0.f, glm::dot(R, view)), 10) * kS);
}
