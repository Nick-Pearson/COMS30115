#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glm/glm.hpp>

using glm::vec3;

class Cubemap
{
public:
	virtual ~Cubemap() {}

	virtual vec3 GetCubemapColour(const vec3& direction) const = 0;
};

#endif
