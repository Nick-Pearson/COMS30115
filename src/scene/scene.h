#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

class Mesh;
class Camera;
class Cubemap;

using glm::mat4;
using glm::vec3;
using glm::mat3;

struct Intersection
{
	vec3 position;
	float distance;
	std::shared_ptr<Mesh> mesh;
	int triangleIndex;
};

class Scene
{
public:

	Scene();
	~Scene();

	void Update(float DeltaSeconds);

	bool ClosestIntersection(const vec3& start, const vec3& dir, Intersection& closestIntersection) const;

	void AddMesh(std::shared_ptr<Mesh> mesh) { Meshes.push_back(mesh); }

	vec3 GetEnvironmentColour(const vec3& dir) const;

	Camera* camera;

	Cubemap* environment;

private:
	std::vector<std::shared_ptr<Mesh>> Meshes;
};
#endif
