#include "antialiasing.h"

#include <iostream>
#include <algorithm>

using namespace std;

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;

vec3 performAntiAliasing(vec4 *screen, int x, int y, int width, int height, vec4 colour)
{
  vec3 colorCenter = vec3(colour);

  // Luma at the current fragment
  float lumaCenter = rgb2luma(colorCenter);

  // Luma at the four direct neighbours of the current fragment.
  // TODO: Optimise access

  vec3 yDownColour = colourAtPosition(screen, x, y, width, height, ivec2(0, -1));
  vec3 yUpColour = colourAtPosition(screen, x, y, width, height, ivec2(0, 1));
  vec3 xLeftColour = colourAtPosition(screen, x, y, width, height, ivec2(-1, 0));
  vec3 xRightColour = colourAtPosition(screen, x, y, width, height, ivec2(1, 0));

  float lumaDown = rgb2luma(yDownColour);
  float lumaUp = rgb2luma(yUpColour);
  float lumaLeft = rgb2luma(xLeftColour);
  float lumaRight = rgb2luma(xRightColour);

  // Find the maximum and minimum luma around the current fragment.
  float lumaMin = min(lumaCenter, min( min(lumaDown,lumaUp), min(lumaLeft,lumaRight)));
  float lumaMax = max(lumaCenter, max( max(lumaDown,lumaUp), max(lumaLeft,lumaRight)));

  // Compute the delta.
  float lumaRange = lumaMax - lumaMin;

  // If the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA.
  if(lumaRange < max(EDGE_THRESHOLD_MIN,lumaMax*EDGE_THRESHOLD_MAX)){
      return colorCenter;
  }

  return vec3(255, 255, 255);
}

vec3 colourAtPosition(vec4* screen, int x, int y, int width, int height, ivec2 pos) {
  int newY = y + pos[1];

  if (newY < 0) {
    newY = height + newY;
  } else if (newY > (height - 1)) {
    newY = newY % height;
  }

  int newX = x + pos[0];

  if (newX < 0) {
    newX = width + newX;
  } else if (newX > (width - 1)) {
    newX = newX % width;
  }

  return vec3(screen[newY * width + newX]);
}

float rgb2luma(const vec3& rgb) {
  return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}
