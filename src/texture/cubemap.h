#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glm/glm.hpp>

using glm::vec3;

class Cubemap
{
public:
	virtual ~Cubemap() {}

	virtual void GetCubemapColour(const vec3& direction, uint8_t (&colour)[4]) const = 0;
};

#endif
