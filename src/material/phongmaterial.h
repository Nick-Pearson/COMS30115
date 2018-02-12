#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H

#include "material.h"

#include <glm/glm.hpp>

class PhongMaterial : public Material
{
public:
	glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal, const glm::vec3& albedo) override;

	float kD = 1.0f;
	float kS = 0.0f;
};

#endif // !PHONGMATERIAL_H

