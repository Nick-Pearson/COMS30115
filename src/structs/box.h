#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>

#include <algorithm>

struct Box
{
	Box() :
		isValid(false)
	{}
	
	void operator+=(const glm::vec3& other)
	{
		if (!isValid)
		{
			min = other;
			max = other;
			isValid = true;
			return;
		}

		max.x = std::max(max.x, other.x);
		max.y = std::max(max.y, other.y);
		max.z = std::max(max.z, other.z);

		min.x = std::min(min.x, other.x);
		min.y = std::min(min.y, other.y);
		min.z = std::min(min.z, other.z);
	}

	bool DoesIntersect(const glm::vec3& start, glm::vec3 dir) const;

	glm::vec3 min;
	glm::vec3 max;

private:
	bool isValid = false;
};

#endif