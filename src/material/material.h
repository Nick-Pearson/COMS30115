#ifndef MATERIAL_H
#define MATERIAL_H

#include "material.h"

#include <glm/glm.hpp>

class Material
{
public:

	virtual glm::vec3 CalculateBRDF(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) = 0;
};

#endif
