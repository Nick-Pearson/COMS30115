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

  inline glm::vec2 cross2D(const glm::vec2& a, const glm::vec2& b) { return glm::vec2(glm::cross(glm::vec3(a, 1.0f), glm::vec3(b, 1.0f))); }

  inline bool isNearlyZero(const float val) { return abs(val) < 0.001f;}

  // Angles

  // converts and angle in degrees to radians
  inline float ToRads(float angle) { return angle * PI / 180.0f; }

  // converts and angle in radians to degrees
  inline float ToDegs(float angle) { return angle * 180.0f / PI; }

  // Interpolation

  template<typename T>
  inline void interpolate(const T& a, const T& b, std::vector<T>& result)
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
  }

  template<>
  inline void interpolate(const glm::ivec2& a, const glm::ivec2& b, std::vector<glm::ivec2>& result)
  {
    int size = result.size();
    glm::vec2 step = glm::vec2(b - a) / float(std::max(size - 1, 1));
    glm::vec2 current(a);

    for (int i = 0; i < size; ++i)
    {
      result[i] = current;
      current += step;
    }
  }

  template<typename T>
  inline T interpolate(const T& a, const T& b, float alpha)
  {
    return a + (b-a)*alpha;
  }

  template <typename T>
  inline int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }

  void interpolateLine(const glm::ivec2& a, const glm::ivec2& b, std::vector<glm::ivec2>& result);

  // finds the intersection of two lines, returns a distance along the first line
  inline float findLineIntersection(const glm::vec2& p0, const glm::vec2& d0, const glm::vec2& p1, const glm::vec2& d1)
  {
    const glm::vec2 p = p1 - p0;
    return ((p.x*d1.y) - (p.y*d1.x)) / ((d0.x*d1.y) - (d0.y*d1.x));
  }
};

#endif
