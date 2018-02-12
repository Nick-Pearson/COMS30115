#ifndef ANTI_ALIASING_H
#define ANTI_ALIASING_H

#include "antialiasing.h"

#include <glm/glm.hpp>

using glm::vec3;

void performAntiAliasing();

float rgb2luma(const vec3& rgb);

#endif
