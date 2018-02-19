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

bool AMath::IsPointWithinTriangle(const glm::ivec2& p, const glm::ivec2& v0, const glm::ivec2& v1, const glm::ivec2& v2)
{
	auto& edgeFunction = [](const glm::ivec2& a, const glm::ivec2& b, const glm::ivec2& c)
	{
		return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
	};

	return edgeFunction(v0, v1, p) &&
	  edgeFunction(v1, v2, p) &&
	  edgeFunction(v2, v0, p);
}
