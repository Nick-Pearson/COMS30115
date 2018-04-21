#include "scene.h"

#include "camera.h"
#include "../mesh/mesh.h"
#include "../texture/texturecubemap.h"
#include "../surface/implicitsurface.h"

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
	}, closestIntersection);
}

bool Scene::ShadowIntersection(const vec3& start, const vec3& dir, Intersection& firstIntersection) const
{
	return IntersectScene_Internal(start, dir, [&](float t, const Intersection& curIntersection) {
		return t < 1.0f;
	}, firstIntersection, true);
}

bool Scene::Raymarch(const vec3& start, const vec3& dir, Intersection& closestGeometry) const
{
	return false;
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
  dir = glm::normalize(dir);

	for (const std::shared_ptr<Mesh> mesh : Meshes)
	{
		if (!mesh->bounds.DoesIntersect(start, dir)) continue;

		for (size_t i = 0; i < mesh->Triangles.size(); i++)
		{
			Triangle& triangle = mesh->Triangles[i];

			// Dot product optimisation
			if (glm::dot(triangle.normal, dir) >= 0.0f)
			{
				continue;
			}

			vec3 v0 = mesh->Verticies[triangle.v0].position;
			vec3 v1 = mesh->Verticies[triangle.v1].position;
			vec3 v2 = mesh->Verticies[triangle.v2].position;

			vec3 e1 = v1 - v0;
			vec3 e2 = v2 - v0;

			vec3 b = start - v0;

			mat3 A(-dir, e1, e2);
			const float detA = glm::determinant(A);

			// solve t first and check if it is valid
			const float t = glm::determinant(glm::column(A, 0, b)) / detA;

			if (t < 0.0f || !Predicate(t, const_cast<const Intersection&>(outIntersection)))
				continue;

			const float u = glm::determinant(glm::column(A, 1, b)) / detA;
			const float v = glm::determinant(glm::column(A, 2, b)) / detA;

			if (u >= 0 && v >= 0 && u + v <= 1)
			{
				outIntersection = Intersection(start + (t * dir), t, mesh, (int)i);

				if (terminateOnValidIntersection)
					return true;
			}
		}
	}

  for (const std::shared_ptr<ImplicitSurface> surf : Surfaces)
  {
    float t = 0.0f;
    glm::vec3 normal;
    if (!surf->Intersect(start, dir, t, normal)) continue;
    
    if(t < 0.0f || !Predicate(t, const_cast<const Intersection&>(outIntersection)))
      continue;

    outIntersection = Intersection(start + (t * dir), t, surf, normal);

    if (terminateOnValidIntersection)
      return true;
  }

  return outIntersection.isValid();
}
