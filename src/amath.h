#ifndef AMATH_H
#define AMATH_H

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>

//set of additonal maths functions and defines
#define PI 3.141593f
#define TWO_PI 6.283185f

namespace AMath
{
  template<typename T>
  inline T square(T v) { return v*v; }
/*
  template<typename T>
  inline void interpolate(T a, T b, std::vector<T>& result)
  {
    if(result.size() == 1)
    {
      result[1] = (a + b) * 0.5f;
      return;
    }

    int size = result.size();

    for(int i = 0; i < size; ++i)
    {
      result[i] = a + ((b - a) * ((float)i / (float)(size-1)));
    }
  }*/

  inline void interpolate(const glm::ivec2& a, const glm::ivec2& b, std::vector<glm::ivec2>& result)
  {
    int size = result.size();
    glm::vec2 step = glm::vec2(b-a) / float(std::max(size-1, 1));
    glm::vec2 current(a);

    for(int i = 0; i < size; ++i)
    {
      result[i] = current;
      current += step;
    }
  }

  template <typename T> 
  inline int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }

  inline void interpolateLine(const glm::ivec2& a, const glm::ivec2& b, std::vector<glm::ivec2>& result) {
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
};

#endif
