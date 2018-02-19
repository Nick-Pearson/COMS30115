#ifndef AMATH_H
#define AMATH_H

#include <glm/glm.hpp>

#include <algorithm>

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
};

#endif
