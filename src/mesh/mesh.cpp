#include "mesh.h"

#include "../material/phongmaterial.h"

Mesh::Mesh(const vector<Vertex>& inVerticies, const vector<Triangle>& inTriangles) :
  Verticies(inVerticies), Triangles(inTriangles)
{
	CacheNormals();
	CalculateBounds();

  /*for (Triangle& tri : Triangles)
  {
    int vals[3] = { tri.v0, tri.v1, tri.v2 };
    for(int i = 0; i < 3; ++i)
    {
      if(vals[i] < 0 || vals[i] )
    }
    printf("Invalid Mesh\n", );
  }
*/
	material = std::shared_ptr<Material>(new PhongMaterial);
}

void Mesh::CacheNormals()
{
	for (Triangle& tri : Triangles)
	{
		const vec3 e1 = Verticies[tri.v1].position - Verticies[tri.v0].position;
		const vec3 e2 = Verticies[tri.v2].position - Verticies[tri.v0].position;
		tri.normal = glm::normalize(glm::cross(e2, e1));
	}
}

void Mesh::CalculateBounds()
{
	bounds = Box();

	for (const Vertex& vert : Verticies)
		bounds += vert.position;
}
