#ifndef  MESH_H
#define  MESH_H

#include <vector>

#include <glm/glm.hpp>

#include <memory>

#include "../structs/box.h"
#include "../misc.h"

using namespace glm;
using namespace std;

class Material;

// position and other metadata for a single vertex
struct Vertex
{
  Vertex() {}

  Vertex(const vec3& inPosition) :
    position(inPosition)
  {}

  Vertex(const vec3& inPosition, const vec2& inUV0) :
    position(inPosition), uv0(inUV0)
  {}


  vec3   position = vec3(0.0f, 0.0f, 0.0f);
  vec2   uv0      = vec2(0.0f, 0.0f);

  // These operators need to be updated when the above data is changed

  void operator*=(float factor)
  {
    position *= factor;
    uv0 *= factor;
  }

  void operator+=(const Vertex& other)
  {
    position += other.position;
    uv0 += other.uv0;
  }

  void operator-=(const Vertex& other)
  {
    position -= other.position;
    uv0 -= other.uv0;
  }

  // these do not need to change
  Vertex operator*(float factor) const
  {
    Vertex returnValue = *this;
    returnValue *= factor;
    return returnValue;
  }

  Vertex operator+(const Vertex& other) const
  {
    Vertex returnValue = *this;
    returnValue += other;
    return returnValue;
  }

  Vertex operator-(const Vertex& other) const
  {
    Vertex returnValue = *this;
    returnValue -= other;
    return returnValue;
  }
};

// the vertex indicies and colour value of a triangle
class Triangle
{
public:
	int v0, v1, v2;
	vec3 normal, tangent, bitangent;

	Triangle(int v0, int v1, int v2)
		: v0(v0), v1(v1), v2(v2), normal(glm::vec3()), tangent(glm::vec3()), bitangent(glm::vec3())
	{}

  inline void CalculateNormal(const glm::vec3& v0Pos, const glm::vec3& v1Pos, const glm::vec3& v2Pos)
  {
    const vec3 e1 = v1Pos - v0Pos;
    const vec3 e2 = v2Pos - v0Pos;
    normal = glm::normalize(glm::cross(e2, e1));

    //calculate the tangent and bitangent
    Misc::CalcTangentsFromNormal(normal, tangent, bitangent);
  }
};

class Mesh
{
public:

  // constructor that takes a set of verticies and triangles to copy (any invalid triangles will be dropped)
  Mesh(const vector<Vertex>& inVerticies, const vector<Triangle>& inTriangles);

  virtual ~Mesh() {}

  void Translate(const glm::vec3& translation);

  void Rotate(const glm::vec3& eulerAngles);

  void Scale(const float scaleFactor) { return Scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor)); }
  void Scale(const glm::vec3& scaleFactor);

  void FlipNormals();

  vector<Vertex> Verticies;
  vector<Triangle> Triangles;

  Box bounds;

  std::shared_ptr<Material> GetMaterial(int32 triangleIndex) const;

  void SetMaterial(std::shared_ptr<Material> Material);
  void SetMaterials(const std::vector<std::shared_ptr<Material>>& Materials, const std::vector<uint8_t>& MaterialIndicies);
  void SetMaterialOnTriangle(std::shared_ptr<Material> Material, int triangleIdx);

private:

  std::vector<std::shared_ptr<Material>> materials;
  std::vector<uint8_t> materialIndicies;

	void CacheNormals();
	void CalculateBounds();
};

#endif // ! MESH_H