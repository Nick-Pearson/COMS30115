#include "meshfactory.h"

#include "mesh.h"

#include <iostream>
#include <fstream>

//private functions for loading files
shared_ptr<Mesh> LoadOBJFile(std::string path);

shared_ptr<Mesh> MeshFactory::LoadFromFile(const std::string& filepath)
{
  MeshType type = MeshType::UNKOWN;

  size_t file_extension_idx = filepath.find_last_of('.');
  if(file_extension_idx == std::string::npos)
  {
    std::cout << "Unable to parse file extension from filename" << std::endl;
    return nullptr;
  }

  std::string file_extension = filepath.substr(file_extension_idx);

  const std::string fullpath = "./resources/meshes/" + filepath;

  if(file_extension == ".obj")
    type = MeshType::OBJ;

  shared_ptr<Mesh> meshptr;

  switch (type) {
    case MeshType::OBJ:
      meshptr = LoadOBJFile(fullpath);
      break;

    default:
      std::cout << "Unsupported file type '" << file_extension << "'" << std::endl;
      break;
  }

  return meshptr;
}

shared_ptr<Mesh> LoadOBJFile(std::string path)
{
  ifstream ifs (path.c_str());

  if(!ifs.good())
  {
    std::cout << "Unable to open file '" << path << "'" << std::endl;
    return nullptr;
  }

  //read the file line by line
  std::string line;
  line.reserve(512);
  line.clear();

  vector<Vertex> verts;
  vector<Triangle> triangles;

  while(std::getline(ifs, line))
  {
    if(line.size() == 0)
      continue;

    char type = line[0];

    std::string data = line.size() > 1 ? line.substr(1) : "";
    line.erase(line.begin(), std::find_if(line.begin(), line.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));

    if(type == '#')
    {
      continue;
    }
    else if(type == 'v')
    {
      if(data[0] == 'n')
      {
        // TODO: Implement vertex normals
        continue;
      }

      float x,y,z;
      if(3 != sscanf(data.c_str(), "%f %f %f", &x, &y, &z))
      {
        std::cout << "Corrupt OBJ file vertex '" << data << "'" << std::endl;
        return nullptr;
      }

      verts.push_back(Vertex(glm::vec3(x,-y,z) * 0.1f, glm::vec2(0.0f, 0.0f)));
    }
    else if(type == 'f')
    {
      int v0,v1,v2,dummy;
      if(3 != sscanf(data.c_str(), "%d %d %d", &v0, &v1, &v2) &&
          6 != sscanf(data.c_str(), "%d/%d %d/%d %d/%d", &v0, &dummy, &v1, &dummy, &v2, &dummy) &&
          6 != sscanf(data.c_str(), "%d//%d %d//%d %d//%d", &v0, &dummy, &v1, &dummy, &v2, &dummy) &&
          9 != sscanf(data.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", &v0, &dummy, &dummy, &v1, &dummy, &dummy, &v2, &dummy, &dummy))
      {
        std::cout << "Corrupt OBJ file face '" << data << "'" << std::endl;
        return nullptr;
      }

      if(v0 < 0) v0 = verts.size() + v0 + 1;
      if(v1 < 0) v1 = verts.size() + v1 + 1;
      if(v2 < 0) v2 = verts.size() + v2 + 1;

      triangles.push_back(Triangle(v0 - 1, v1 - 1, v2 - 1, glm::vec3(1.0f, 1.0f, 1.0f)));
    }

    line.clear();
  }

  return shared_ptr<Mesh>(new Mesh(verts, triangles));
}

namespace
{
  void AddQuad(int v1, int v2, int v3, int v4, const vec3& colour, vector<Triangle>& triangles)
  {
    triangles.push_back(Triangle(v1, v3, v2, colour));
    triangles.push_back(Triangle(v2, v3, v4, colour));
  }
}

shared_ptr<Mesh> MeshFactory::GetCornelRoom()
{
	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
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
	triangles.push_back( Triangle( C, B, A, white) );
	triangles.push_back( Triangle( C, D, B, white) );

	// Left wall
	triangles.push_back( Triangle( A, E, C, red ) );
	triangles.push_back( Triangle( C, E, G, red) );

	// Right wall
	triangles.push_back( Triangle( F, B, D, green ) );
	triangles.push_back( Triangle( H, F, D, green) );

	// Ceiling
	triangles.push_back( Triangle( E, F, G, white) );
	triangles.push_back( Triangle( F, H, G, white) );

	// Back wall
	triangles.push_back( Triangle( G, D, C, white ) );
	triangles.push_back( Triangle( G, H, D, white ) );

	for (Vertex& vert : verts)
	{
		vert.position *= 2 / L;
		vert.position -= vec3(1, 1, 1);

		vert.position.x *= -1;
		vert.position.y *= -1;
	}

  return shared_ptr<Mesh>(new Mesh(verts, triangles));
}

shared_ptr<Mesh> MeshFactory::GetCube(const vec3& colour, const glm::vec3& pos, const glm::vec3& scale)
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
  AddQuad(0, 1, 4, 5, colour, triangles);
  AddQuad(3, 2, 7, 6, colour, triangles);

  // sides
  AddQuad(1, 0, 3, 2, colour, triangles);
  AddQuad(4, 5, 6, 7, colour, triangles);
  AddQuad(2, 0, 6, 4, colour, triangles);
  AddQuad(1, 3, 5, 7, colour, triangles);

  for (Vertex& vert : verts)
  {
	  vert.position *= scale;
	  vert.position += pos;
  }

  return shared_ptr<Mesh>(new Mesh(verts, triangles));
}
