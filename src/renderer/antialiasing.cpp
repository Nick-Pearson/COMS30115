#include "antialiasing.h"

#include <stdio.h>

void performAntiAliasing()
{

}

float rgb2luma(const vec3& rgb) {
  return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}
