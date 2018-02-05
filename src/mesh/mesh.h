#include <vector>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

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
	{
	}
};

class Mesh
{
public:
  // constructor that takes a set of verticies and triangles to copy (any invalid triangles will be dropped)
  Mesh(const vector<Vertex>& inVerticies, const vector<Triangle>& inTriangles);

  virtual ~Mesh() {}

  vector<Vertex> Verticies;
  vector<Triangle> Triangles;

private:
	void CacheNormals();
};
