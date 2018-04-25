#ifndef SCENE_H
#define SCENE_H

#include "../mesh/mesh.h"

#include <memory>
#include <vector>

#include <glm/glm.hpp>

class Camera;
class Cubemap;
class Light;
class ImplicitSurface;
class Material;

using glm::mat4;
using glm::vec3;
using glm::mat3;

struct Intersection
{
	Vertex vertexData;
	float distance = 0.0f;

  Intersection() {}

  Intersection(const Vertex& inVertexData, float inDist, std::shared_ptr<Mesh> inMesh, int inTriIdx) :
    vertexData(inVertexData), distance(inDist), mesh(inMesh), triangleIndex(inTriIdx)
  {}

  Intersection(const Vertex& inVertexData, float inDist, std::shared_ptr<ImplicitSurface> inSurf, vec3 inNormal) :
    vertexData(inVertexData), distance(inDist), surf(inSurf), normal(inNormal)
  {}

  inline bool isValid() const { return mesh || surf; }
  std::shared_ptr<Material> GetMaterial() const;
  vec3 GetNormal() const;

private:

	std::shared_ptr<Mesh> mesh = nullptr;
	int triangleIndex = -1;

  std::shared_ptr<ImplicitSurface> surf = nullptr;
  vec3 normal;
};

class Scene
{
public:

	Scene();
	~Scene();

	void Update(float DeltaSeconds);

	// returns the closest facing object along the Ray
	bool ClosestIntersection(const vec3& start, const vec3& dir, Intersection& closestIntersection) const;

	// returns any intersecting object along the ray
	bool ShadowIntersection(const vec3& start, const vec3& dir, Intersection& firstIntersection) const;

	bool Raymarch(const vec3& start, const vec3& dir, Intersection& closestGeometry) const;

	void AddMesh(std::shared_ptr<Mesh> mesh) { if(mesh) Meshes.push_back(mesh); }
	inline const std::vector<std::shared_ptr<Mesh>>* GetMeshes() const { return &Meshes; }

  void AddLight(std::shared_ptr<Light> light) { if(light) Lights.push_back(light); }
  inline const std::vector<std::shared_ptr<Light>>* GetLights() const { return &Lights; }

  void AddSurface(std::shared_ptr<ImplicitSurface> Surface);
  inline const std::vector<std::shared_ptr<ImplicitSurface>>* GetSurfaces() const { return &Surfaces; }

	vec3 GetEnvironmentColour(const vec3& dir) const;

	Camera* camera;

	Cubemap* environment;

private:

	std::vector<std::shared_ptr<Mesh>> Meshes;
  std::vector<std::shared_ptr<Light>> Lights;
  std::vector<std::shared_ptr<ImplicitSurface>> Surfaces;

	// querys the scene for intersections, will return if the predicate function returns true
	template<typename Func>
	bool IntersectScene_Internal(const vec3& start, vec3 dir, Func Predicate, Intersection& outIntersection, bool terminateOnValidIntersection = false) const;
};
#endif
