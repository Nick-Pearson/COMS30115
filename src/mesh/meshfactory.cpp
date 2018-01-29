#include "meshfactory.h"

#include "mesh.h"

namespace
{
  void AddQuad(int v1, int v2, int v3, int v4, const vec3& colour, vector<Triangle>& triangles)
  {
    triangles.push_back(Triangle(v1, v2, v3, colour));
    triangles.push_back(Triangle(v2, v3, v4, colour));
  }
}

shared_ptr<Mesh> MeshFactory::GetCornelRoom()
{
	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  0.75f, 0.75f, 0.75f );

  float L = 555;			// Length of Cornell Box side.

  vector<Vertex> verts;
  verts.reserve(8);

	verts.push_back(Vertex(vec3(L,0,0)));
	verts.push_back(Vertex(vec3(0,0,0)));
	verts.push_back(Vertex(vec3(L,0,L)));
	verts.push_back(Vertex(vec3(0,0,L)));
	verts.push_back(Vertex(vec3(L,L,0)));
	verts.push_back(Vertex(vec3(0,L,0)));
	verts.push_back(Vertex(vec3(L,L,L)));
	verts.push_back(Vertex(vec3(0,L,L)));

  // indicies
  int A = 0;
  int B = 1;
  int C = 2;
  int D = 3;
  int E = 4;
  int F = 5;
  int G = 6;
  int H = 7;

  vector<Triangle> triangles;
  triangles.reserve(5 * 2 * 3);

	// Floor:
	triangles.push_back( Triangle( C, B, A, green ) );
	triangles.push_back( Triangle( C, D, B, green ) );

	// Left wall
	triangles.push_back( Triangle( A, E, C, purple ) );
	triangles.push_back( Triangle( C, E, G, purple ) );

	// Right wall
	triangles.push_back( Triangle( F, B, D, yellow ) );
	triangles.push_back( Triangle( H, F, D, yellow ) );

	// Ceiling
	triangles.push_back( Triangle( E, F, G, cyan ) );
	triangles.push_back( Triangle( F, H, G, cyan ) );

	// Back wall
	triangles.push_back( Triangle( G, D, C, white ) );
	triangles.push_back( Triangle( G, H, D, white ) );

  return make_shared<Mesh>(verts, triangles);
}

shared_ptr<Mesh> MeshFactory::GetCube(const vec3& colour)
{
  vector<Vertex> verts;
  verts.reserve(8);

  const float min = -0.5f;
  const float max = 0.5f;

	verts.push_back(Vertex(vec3(min, min, min)));
	verts.push_back(Vertex(vec3(max, min, min)));
	verts.push_back(Vertex(vec3(min, max, min)));
	verts.push_back(Vertex(vec3(max, max, min)));

	verts.push_back(Vertex(vec3(min, min, max)));
	verts.push_back(Vertex(vec3(max, min, max)));
	verts.push_back(Vertex(vec3(min, max, max)));
	verts.push_back(Vertex(vec3(max, max, max)));

  vector<Triangle> triangles;
  triangles.reserve(12);

  // Top and bottom
  AddQuad(0, 1, 2, 3, colour, triangles);
  AddQuad(4, 5, 6, 7, colour, triangles);

  // sides
  AddQuad(0, 1, 4, 5, colour, triangles);
  AddQuad(1, 2, 5, 6, colour, triangles);
  AddQuad(2, 3, 6, 7, colour, triangles);
  AddQuad(3, 0, 7, 4, colour, triangles);

  return make_shared<Mesh>(verts, triangles);
}
