#include <vector>

#include <glm/glm.hpp>

#include <memory>

#include "../structs/box.h"

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

  void operator*=(float factor)
  {
    position *= factor;
    uv0 *= factor;
  }

  Vertex operator*(float factor) const
  {
    Vertex returnValue = *this;
    returnValue *= factor;
    return returnValue;
  }

  void operator+=(const Vertex& other)
  {
    position += other.position;
    uv0 += other.uv0;
  }

  Vertex operator+(const Vertex& other)
  {
    Vertex returnValue = *this;
    returnValue += other;
    return returnValue;
  }
};

// the vertex indicies and colour value of a triangle
class Triangle
{
public:
	int v0, v1, v2;
	vec3 colour;
	vec3 normal;

	Triangle(int v0, int v1, int v2, glm::vec3 colour)
		: v0(v0), v1(v1), v2(v2), colour(colour), normal(glm::vec3())
	{}
};

class Mesh
{
public:
  // constructor that takes a set of verticies and triangles to copy (any invalid triangles will be dropped)
  Mesh(const vector<Vertex>& inVerticies, const vector<Triangle>& inTriangles);

  virtual ~Mesh() {}

  vector<Vertex> Verticies;
  vector<Triangle> Triangles;

  Box bounds;

  std::shared_ptr<Material> material;

private:
	void CacheNormals();
	void CalculateBounds();
};
