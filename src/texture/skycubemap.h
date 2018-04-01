#ifndef SKYCUBEMAP_H
#define SKYCUBEMAP_H

#include "cubemap.h"

class SkyCubemap : public Cubemap
{
public:

	void GetCubemapColour(const vec3& direction, uint8_t (&colour)[4]) const override;
};

#endif
