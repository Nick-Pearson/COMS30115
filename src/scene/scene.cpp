#include "scene.h"

#include "camera.h"
#include "../misc.h"
#include "../mesh/mesh.h"
#include "../texture/texturecubemap.h"
#include "../surface/implicitsurface.h"
#include "../structs/KDtree.h"

#include <glm/gtc/matrix_access.hpp>

std::shared_ptr<Material> Intersection::GetMaterial() const
{
  if (mesh)
  {
    return mesh->GetMaterial(triangleIndex);
  }
  else if(surf)
  {
    return surf->GetMaterial();
  }

  return nullptr;
}

void Intersection::GetNormals(vec3& outNormal, vec3& outTangent, vec3& outBitangent) const
{
  outNormal = GetNormal();

  if (mesh)
  {
    outTangent = mesh->Triangles[triangleIndex].tangent;
    outBitangent = mesh->Triangles[triangleIndex].bitangent;
    return;
  }

  Misc::CalcTangentsFromNormal(normal, outTangent, outBitangent);
  return;
}

vec3 Intersection::GetNormal() const
{
  if (mesh)
  {
    return mesh->Triangles[triangleIndex].normal;
  }

  return normal;
}

Scene::Scene()
{
#if RAYTRACER
	camera = new Camera(55.0f, glm::vec3(0, 0, -2.4));
#else
	camera = new Camera(65.0f, glm::vec3(0, 0, -3.4));
#endif

	environment = new TextureCubemap("skyboxes/Maskonaive");
}

Scene::~Scene()
{
	delete camera;
	delete environment;
}

void Scene::Update(float DeltaSeconds)
{
	camera->Update(DeltaSeconds);
}


bool Scene::ClosestIntersection(const vec3& start, const vec3& dir, Intersection& closestIntersection) const
{
	closestIntersection.distance = std::numeric_limits<float>::max();

	return IntersectScene_Internal(start, dir, [&](float t, const Intersection& curIntersection) {
		return t < curIntersection.distance;
	}, closestIntersection, false);
}

bool Scene::ShadowIntersection(const vec3& start, const vec3& dir, Intersection& firstIntersection) const
{
  const float len = glm::length(dir);

	return IntersectScene_Internal(start, glm::normalize(dir), [&](float t, const Intersection& curIntersection) {
		return t < len;
	}, firstIntersection, true);
}

void Scene::AddSurface(std::shared_ptr<ImplicitSurface> Surface)
{
  if (!Surface) return;

#if RAYTRACER
  Surfaces.push_back(Surface);
#else
  AddMesh(Surface->GenerateMesh());
#endif
}

vec3 Scene::GetEnvironmentColour(const vec3& dir) const
{
	if(environment)
	{
		uint8_t bytes[4];
		environment->GetCubemapColour(dir, bytes);
		return vec3((float)bytes[0] / 255.0f, (float)bytes[1] / 255.0f, (float)bytes[2] / 255.0f);
	}

	return vec3(0.0f, 0.0f, 0.0f);
}

template<typename Func>
bool Scene::IntersectScene_Internal(const vec3& start, vec3 dir, Func Predicate, Intersection& outIntersection, bool terminateOnValidIntersection /*= false*/) const
{
  for (const std::shared_ptr<Mesh> mesh : Meshes)
  {
    if(!IntersectScene_Internal_KDNode<Func>(start, dir, mesh, Predicate, outIntersection, mesh->RootNode)) continue;

    Triangle& triangle = mesh->Triangles[outIntersection.triangleIndex];
    const glm::vec3 P = start + (outIntersection.distance * dir);

    //calculate barycentric coordinates
    glm::vec3 v0 = mesh->Verticies[triangle.v0].position;
    glm::vec3 v1 = mesh->Verticies[triangle.v1].position;
    glm::vec3 v2 = mesh->Verticies[triangle.v2].position;

    glm::mat3 R(v0, v1, v2);
    const float detR = glm::determinant(R);

    glm::vec3 coords;
    coords.x = glm::determinant(glm::column(R, 0, P)) / detR;
    coords.y = glm::determinant(glm::column(R, 1, P)) / detR;
    coords.z = glm::determinant(glm::column(R, 2, P)) / detR;

    const Vertex vertexData = (mesh->Verticies[triangle.v0] * coords.x) + (mesh->Verticies[triangle.v1] * coords.y) + (mesh->Verticies[triangle.v2] * coords.z);

    outIntersection = Intersection(vertexData, outIntersection.distance, mesh, outIntersection.triangleIndex);

    if (terminateOnValidIntersection)
      return true;
  }

  for (const std::shared_ptr<ImplicitSurface> surf : Surfaces)
  {
    float t = 0.0f;
    glm::vec3 normal;
    if (!surf->Intersect(start, dir, t, normal) ||
      t < 0.0f ||
      !Predicate(t, const_cast<const Intersection&>(outIntersection)))
      continue;

    outIntersection = Intersection(start + (t * dir), t, surf, normal);

    if (terminateOnValidIntersection)
      return true;
  }

  return outIntersection.isValid();
}

template<typename Func>
bool Scene::IntersectScene_Internal_KDNode(const vec3& start, vec3 dir, const std::shared_ptr<Mesh> mesh, Func Predicate, Intersection& outIntersection, const KDNode* node) const
{
  if (!node->boundingBox.DoesIntersect(start, dir)) return false;
  
  if (node->child1 && node->child2)
  {
    bool hitLeft = IntersectScene_Internal_KDNode(start, dir, mesh, Predicate, outIntersection, node->child1);
    bool hitRight = IntersectScene_Internal_KDNode(start, dir, mesh, Predicate, outIntersection, node->child2);
    return hitLeft || hitRight;
  }

  bool isHit = false;
  for (int i=0; i<node->trianglesIndices.size(); i++) {
    if (CalcIntersectionInternal(start, dir, mesh, mesh->Triangles[node->trianglesIndices[i]], outIntersection)) {
      isHit = true;
      outIntersection.triangleIndex = node->trianglesIndices[i];
    }
  }
  return isHit;
}

static bool CalcIntersectionInternal (const glm::vec3& start, const glm::vec3& direction, const std::shared_ptr<Mesh> mesh, const Triangle& triangle, Intersection &intersection)
{
  // Dot product optimisation
  if (glm::dot(triangle.normal, direction) >= 0.0f)
    return false;

  glm::vec3 v0 = mesh->Verticies[triangle.v0].position;
  glm::vec3 v1 = mesh->Verticies[triangle.v1].position;
  glm::vec3 v2 = mesh->Verticies[triangle.v2].position;

  glm::vec3 e1 = v1 - v0;
  glm::vec3 e2 = v2 - v0;
  glm::vec3 b = start - v0;

  glm::mat3 A(-direction, e1, e2);
  const float detA = glm::determinant(A);

  glm::vec3 x;

  x.x = glm::determinant(glm::column(A, 0, b)) / detA;
  x.y = glm::determinant(glm::column(A, 1, b)) / detA;
  x.z = glm::determinant(glm::column(A, 2, b)) / detA;

  if (x.y >= 0 && x.z >= 0 && x.y + x.z <= 1 && x.x >= 0
    && x.x < intersection.distance) {
    // There is an intersection and it is better than the previous best.
    // intersection.position = start + x.x * direction;
    intersection.distance = x.x;
    return true;
  }
  return false;
}
