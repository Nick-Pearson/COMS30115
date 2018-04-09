#pragma once

#include <vector>

namespace Misc
{
  template<typename T>
  inline void RemoveSwap(std::vector<T>& vector, int idx)
  {
    if (idx < 0 || idx >= vector.size())
      return;

    if (idx + 1 != vector.size())
    {
      std::swap(vector[vector.size()], vector[idx]);
    }
    vector.pop_back();
  }
};