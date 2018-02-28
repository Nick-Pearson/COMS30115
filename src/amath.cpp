#include "amath.h"

void AMath::interpolateLine(const glm::ivec2& a, const glm::ivec2& b, std::vector<glm::ivec2>& result)
{
	const float deltaX = b.x - a.x;
	const float deltaY = b.y - a.y;

	if (deltaX == 0) {
		int i = 0;
		for (int y = a.y; y < b.y; y++) {
			result[i] = glm::vec2(a.x, y);
			i++;
		}
		return;
	}

	const float deltaError = std::abs(deltaY / deltaX);

	float error = 0.0;
	int y = a.y;


	int i = 0;
	for (int x = a.x; x < b.x; x++) {
		result[i] = glm::vec2(x, y);
		error = error + deltaError;
		while (error >= 0.5) {
			y = y + sgn(deltaY) * 1;
			error = error - 1.0;
		}
		i++;
	}
}