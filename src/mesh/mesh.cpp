#include "mesh.h"

#include "../material/material.h"
#include "../amath.h"
#include "../structs/KDTree.h"

#include <glm/gtx/transform.hpp>

Mesh::Mesh(const vector<Vertex>& inVerticies, const vector<Triangle>& inTriangles) :
  Verticies(inVerticies), Triangles(inTriangles)
{
	CacheNormals();
  CalculateBounds();
  BuildKDTree();

	SetMaterial(std::shared_ptr<Material>(new Material(glm::vec3(0.6f, 0.6f, 0.6f))));
}

Mesh::~Mesh()
{
  delete RootNode;
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

  CacheNormals();
  CalculateBounds();
  BuildKDTree();
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

std::shared_ptr<Material> Mesh::GetMaterial(int32 triangleIndex) const
{
  if (materials.size() == 0) return nullptr;
  else if (materials.size() == 1) return materials[0];
  else return materials[materialIndicies[triangleIndex]];
}

void Mesh::SetMaterial(std::shared_ptr<Material> Material)
{
  materials.clear();
  materialIndicies.clear();

  materials.push_back(Material);
}

void Mesh::SetMaterials(const std::vector<std::shared_ptr<Material>>& Materials, const std::vector<uint8_t>& MaterialIndicies)
{
  if (Materials.size() == 0) return;
  else if (Materials.size() == 1) return SetMaterial(Materials[0]);

  materials = Materials;
  materialIndicies = MaterialIndicies;
}

void Mesh::SetMaterialOnTriangle(std::shared_ptr<Material> Material, int triangleIdx)
{
  if (materials.size() == 0) return SetMaterial(Material);
  if (materials.size() == 1 && materials[0] == Material) return;

  // find or add the material
  int materialIdx;
  auto it = std::find(materials.begin(), materials.end(), Material);
  if (it != materials.end())
  {
    materialIdx = it - materials.begin();
  }
  else
  {
    materialIdx = materials.size();
    materials.push_back(Material);
  }

  // if we previously had one material, initialize the list
  if (materialIndicies.size() == 0)
  {
    materialIndicies = std::vector<uint8_t>(Triangles.size());
  }

  materialIndicies[triangleIdx] = materialIdx;
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

  UpdateKDTreeBounds();
}

void Mesh::BuildKDTree()
{
  std::vector<int> trianglesIndices;
  trianglesIndices.reserve(Triangles.size());

  for (int i = 0; i < Triangles.size(); i++)
  {
    trianglesIndices.push_back(i);
  }

  delete RootNode;
  RootNode = RootNode->build(this, trianglesIndices, 0);
}

void Mesh::UpdateKDTreeBounds()
{
  if (RootNode) RootNode->UpdateBounds(this);
}
