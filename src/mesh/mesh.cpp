#include "mesh.h"

#include "../material/phongmaterial.h"
#include "../amath.h"

#include <glm/gtx/transform.hpp>

Mesh::Mesh(const vector<Vertex>& inVerticies, const vector<Triangle>& inTriangles, MeshConstructType ConstructType) :
  Verticies(inVerticies), Triangles(inTriangles)
{
  if(!(ConstructType & MeshConstructType::SKIP_CACHE_NORMALS))
	  CacheNormals();

  if (!(ConstructType & MeshConstructType::SKIP_CALC_BOUNDS))
    CalculateBounds();

	material = std::shared_ptr<Material>(new PhongMaterial);
}

void Mesh::Translate(const glm::vec3& translation)
{
  for (Vertex& v : Verticies)
  {
    v.position += translation;
  }

  CalculateBounds();
}

void Mesh::Rotate(const glm::vec3& eulerAngles)
{
  glm::mat4 rotationMatrix;
  rotationMatrix = glm::rotate(rotationMatrix, AMath::ToRads(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, AMath::ToRads(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, AMath::ToRads(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::vec3 center = bounds.GetCenter();

  for (Vertex& v : Verticies)
  {
    v.position = glm::vec3(rotationMatrix * glm::vec4(v.position - center, 1.0f)) + center;
  }

  CalculateBounds();
}

void Mesh::Scale(const glm::vec3& scaleFactor)
{
  glm::vec3 center = bounds.GetCenter();

  for (Vertex& v : Verticies)
  {
    v.position = ((v.position - center) * scaleFactor) + center;
  }

  CalculateBounds();
}

void Mesh::FlipNormals()
{
  for (Triangle& tri : Triangles)
  {
    std::swap(tri.v0, tri.v1);
  }

  CacheNormals();
}

void Mesh::CacheNormals()
{
	for (Triangle& tri : Triangles)
	{
    tri.CalculateNormal(Verticies[tri.v0].position, Verticies[tri.v1].position, Verticies[tri.v2].position);
	}
}

void Mesh::CalculateBounds()
{
	bounds = Box();

	for (const Vertex& vert : Verticies)
		bounds += vert.position;
}
