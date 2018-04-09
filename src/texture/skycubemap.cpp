#include "skycubemap.h"

void SkyCubemap::GetCubemapColour(const vec3& direction, uint8_t (&colour)[4]) const
{
	colour[0] = 128;
	colour[1] = 179;
	colour[2] = 255;
	colour[3] = 255;
}
