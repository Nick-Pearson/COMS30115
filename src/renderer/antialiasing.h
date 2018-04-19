#ifndef ANTI_ALIASING_H
#define ANTI_ALIASING_H

#include "antialiasing.h"

#include <glm/glm.hpp>

using glm::ivec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;

vec3 performAntiAliasing(vec4* screen, int x, int y, int width, int height);
vec3 colourAtPosition(vec4* screen, int x, int y, int width, int height, ivec2 pos);

float rgb2luma(const vec3& rgb);

#endif
