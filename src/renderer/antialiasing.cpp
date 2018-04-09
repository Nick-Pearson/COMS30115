#include "antialiasing.h"

#include <iostream>
#include <algorithm>

using namespace std;

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;

#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#define FXAA_SPAN_MAX     8.0


vec3 performAntiAliasing(vec4 *screen, int x, int y, int width, int height)
{

  vec3 rgbM = colourAtPosition(screen, x, y, width, height, ivec2(0, 0));
  vec3 rgbNW = colourAtPosition(screen, x, y, width, height, ivec2(-1, -1));
  vec3 rgbNE = colourAtPosition(screen, x, y, width, height, ivec2(1, -1));
  vec3 rgbSW = colourAtPosition(screen, x, y, width, height, ivec2(-1, 1));
  vec3 rgbSE = colourAtPosition(screen, x, y, width, height, ivec2(1, 1));

  // Convert from RGB to luma.
  float lumaNW = rgb2luma(rgbNW);
  float lumaNE = rgb2luma(rgbNE);
  float lumaSW = rgb2luma(rgbSW);
  float lumaSE = rgb2luma(rgbSE);
  float lumaM = rgb2luma(rgbM);

  // Gather minimum and maximum luma.
  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  if (lumaMax - lumaMin < lumaMax * EDGE_THRESHOLD_MAX) {
    return rgbM;
  }

  // Sampling is done along the gradient.
  vec2 samplingDirection;
  samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

  // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
  // This results, that brighter areas are less blurred/more sharper than dark areas.
  float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25f * FXAA_REDUCE_MUL, FXAA_REDUCE_MIN);

  // Factor for norming the sampling direction plus adding the brightness influence.
  float minSamplingDirectionFactor = 1.0f / (min(abs(samplingDirection[0]), abs(samplingDirection[1])) + samplingDirectionReduce);

  samplingDirection = min(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
            max(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
            samplingDirection * minSamplingDirectionFactor)) * vec2(1.0f / width, 1.0f / height);

  vec2 position = vec2(x, y);
  vec2 posA = position + samplingDirection * (1.0f/3.0f - 0.5f);
  vec2 posB = position + samplingDirection * (2.0f/3.0f - 0.5f);
  vec3 rgbSampleNeg = colourAtPosition(screen, posA.x, posA.y, width, height, ivec2(0, 0));
  vec3 rgbSamplePos = colourAtPosition(screen, posB.x, posB.y, width, height, ivec2(0, 0));

  vec3 rgbSampleA = (rgbSamplePos + rgbSampleNeg) * 0.5f;

  vec2 posC = position + samplingDirection * -0.5f;
  vec2 posD = position + samplingDirection * 0.5f;
  vec3 rgbSampleNegOuter = colourAtPosition(screen, posC.x, posC.y, width, height, ivec2(0, 0));
  vec3 rgbSamplePosOuter = colourAtPosition(screen, posD.x, posD.y, width, height, ivec2(0, 0));

  vec3 rgbSampleB = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25f + rgbSampleA * 0.5f;

  float lumaSampleTabB = rgb2luma(rgbSampleB);

  if (lumaSampleTabB < lumaMin || lumaSampleTabB > lumaMax)
  {
    return rgbSampleA;
  } else {
    return rgbSampleB;
  }
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
