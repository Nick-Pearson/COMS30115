#ifndef SKYCUBEMAP_H
#define SKYCUBEMAP_H

#include "cubemap.h"

class SkyCubemap : public Cubemap
{
public:

	vec3 GetCubemapColour(const vec3& direction) const override;
};

#endif

