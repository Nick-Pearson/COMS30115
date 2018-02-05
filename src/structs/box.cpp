#include "box.h"

bool Box::DoesIntersect(const glm::vec3& start, glm::vec3 dir) const
{
	if (!isValid)
		return false;

	glm::vec3 invdir = 1.0f / dir;

	glm::bvec3 sign = glm::bvec3(invdir.x < 0, invdir.y < 0, invdir.z < 0);
	glm::vec3 bounds[2] = { min, max };

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[sign.x].x - start.x) * invdir.x;
	tmax = (bounds[1 - sign.x].x - start.x) * invdir.x;
	tymin = (bounds[sign.y].y - start.y) * invdir.y;
	tymax = (bounds[1 - sign.y].y - start.y) * invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[sign.z].z - start.z) * invdir.z;
	tzmax = (bounds[1 - sign.z].z - start.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}