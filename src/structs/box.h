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

  void operator+=(const Box& other)
  {
    if (!isValid)
    {
      min = other.min;
      min = other.max;
      isValid = true;
      return;
    }

    *this += other.min;
    *this += other.max;
  }

	bool DoesIntersect(const glm::vec3& start, glm::vec3 dir) const;

  	glm::vec3 GetCenter() const { return 0.5f * (min + max); }
	
    inline void Invalidate() { isValid = false; }

	int longestAxis(glm::vec3 vector) {
    if (vector[0] >= vector[1]) {
      if (vector[0] >= vector[2]) {
        return 0;
      } else {
        return 2;
      }
    } else {
      if (vector[1] >= vector[2]) {
        return 1;
      } else {
        return 2;
      }
    }
  }

  int longestAxis() {
    return longestAxis(max - min);
  }

	glm::vec3 min;
	glm::vec3 max;

private:
	bool isValid = false;
};

#endif