#include "scene.h"

#include "camera.h"
#include "../mesh/mesh.h"

#include <glm/gtc/matrix_access.hpp>

Scene::Scene()
{
	camera = new Camera(250.0f, glm::vec3(0, 0, -2.4));
}

Scene::~Scene()
{
	delete camera;
}

void Scene::Update(float DeltaSeconds)
{
	camera->Update(DeltaSeconds);
}


bool Scene::ClosestIntersection(const vec3& start, const vec3& dir, Intersection& closestIntersection) const
{
	closestIntersection.distance = std::numeric_limits<float>::max();
	closestIntersection.triangleIndex = -1;
	closestIntersection.mesh = nullptr;
	closestIntersection.position = vec3(0, 0, 0);

	for (const std::shared_ptr<Mesh> mesh : Meshes)
	{
		if(!mesh->bounds.DoesIntersect(start, dir)) continue;

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
			float t = glm::determinant(glm::column(A, 0, b)) / detA;

			if (t < 0 || t > closestIntersection.distance)
				continue;

			float u = glm::determinant(glm::column(A, 1, b)) / detA;
			float v = glm::determinant(glm::column(A, 2, b)) / detA;

			if (u >= 0 && v >= 0 && u + v <= 1)
			{
				closestIntersection.distance = t;
				closestIntersection.position = start + (t * dir);
				closestIntersection.mesh = mesh;
				closestIntersection.triangleIndex = i;
			}
		}
	}

	return closestIntersection.triangleIndex != -1;
}