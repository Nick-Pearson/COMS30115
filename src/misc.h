#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Misc
{
  template<typename T>
  inline void RemoveSwap(std::vector<T>& vector, int idx)
  {
    if (idx < 0 || idx >= vector.size())
      return;

    if (idx + 1 != vector.size())
    {
      std::swap(vector[vector.size() - 1], vector[idx]);
    }
    vector.pop_back();
  }

  inline void CalcTangentsFromNormal(const glm::vec3& normal, glm::vec3& tangent, glm::vec3& bitangent)
  {
    const glm::vec3 c1 = glm::cross(normal, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::vec3 c2 = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));

    tangent = glm::normalize((glm::length2(c1) > glm::length2(c2)) ? c1 : c2);
    bitangent = glm::normalize(glm::cross(tangent, normal));
  }
};