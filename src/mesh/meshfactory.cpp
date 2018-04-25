#include "meshfactory.h"

#include "mesh.h"
#include "../material/phongmaterial.h"

#include <iostream>
#include<cctype>
#include <fstream>

#pragma warning(disable:4996)

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

void LoadMTLLib(std::string path, std::vector<std::shared_ptr<Material>>& outMaterials, std::vector<std::string>& outMaterialNames)
{
  ifstream ifs(path.c_str());

  if (!ifs.good())
  {
    std::cout << "Unable to open mtl library '" << path << "'" << std::endl;
    return;
  }

  std::string line;
  line.clear();

  std::shared_ptr<PhongMaterial> curMaterial;

  while (std::getline(ifs, line))
  {
    std::string data = line.size() > 1 ? line.substr(1) : "";
    auto firstSpaceIt = std::find_if(data.begin(), data.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
      });

    if(firstSpaceIt != data.end())
      data.erase(data.begin(), firstSpaceIt + 1);

    if (line.substr(0, 6).compare("newmtl") == 0)
    {
      curMaterial = std::shared_ptr<PhongMaterial>(new PhongMaterial(glm::vec3(0.75f, 0.75f, 0.75f)));
      outMaterials.push_back(curMaterial);
      outMaterialNames.push_back(data);
    }
    else if (line.substr(0, 2).compare("Ns") == 0)
    {
      float val;
      if (3 == sscanf(data.c_str(), "%f", &val))
        curMaterial->specularExponent = val;
    }
    else if (line.substr(0, 2).compare("Kd") == 0)
    {
      float r,g,b;
      if (3 == sscanf(data.c_str(), "%f %f %f", &r, &g, &b))
        curMaterial->albedo = glm::vec3(r,g,b);
    }
    else if (line.substr(0, 2).compare("Ks") == 0)
    {
      float r, g, b;
      if (3 == sscanf(data.c_str(), "%f %f %f", &r, &g, &b))
        curMaterial->specular = glm::vec3(r, g, b);
    }

    line.clear();
  }
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
  line.clear();

  vector<Vertex> verts;
  int textureCoordIdx = 0;
  vector<glm::vec3> vertnormals;
  vector<Triangle> triangles;

  std::vector<std::shared_ptr<Material>> materials;
  std::vector<std::string> materialNames;

  uint8_t curMaterialIndex = 0;
  std::vector<uint8_t> materialIndicies;

  while(std::getline(ifs, line))
  {
    if(line.size() == 0)
      continue;

    char type = line[0];

    std::string data = line.size() > 1 ? line.substr(1) : "";
    auto firstSpaceIt = std::find_if(data.begin(), data.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
      });

    if(firstSpaceIt != data.end())
      data.erase(data.begin(), firstSpaceIt + 1);

    if(type == '#')
    {
      continue;
    }
    else if(type == 'v')
    {
      if(data.size() != 0 && data[0] == 'n')
      {
        float x, y, z;
        if (3 != sscanf(data.c_str() + 2, "%f %f %f", &x, &y, &z))
        {
          std::cout << "Corrupt OBJ file vertex normal '" << data << "'" << std::endl;
          return nullptr;
        }

        vertnormals.push_back(glm::vec3(x, y, z));
      }
      else if (data.size() != 0 && line[1] == 't')
      {
        float x, y;
        if (2 != sscanf(data.c_str(), "%f %f", &x, &y))
        {
          std::cout << "Corrupt OBJ file vertex texture '" << data << "'" << std::endl;
          return nullptr;
        }

        verts[textureCoordIdx].uv0 = glm::vec2(x, y);
        textureCoordIdx++;
      }
      else
      {
        float x, y, z;
        if (3 != sscanf(data.c_str(), "%f %f %f", &x, &y, &z))
        {
          std::cout << "Corrupt OBJ file vertex '" << data << "'" << std::endl;
          return nullptr;
        }

        verts.push_back(Vertex(glm::vec3(x,y,z), glm::vec2(0.0f, 0.0f)));
      }
    }
    else if(type == 'f')
    {
      int v0, v1, v2; // vertex indices
      int uv0 = -1, uv1 = -1, uv2 = -1; // texture coordinate indices
      int vn0 = -1, vn1 = -1, vn2 = -1; // vertex normal indices

      if(3 != sscanf(data.c_str(), "%d %d %d", &v0, &v1, &v2) &&
          6 != sscanf(data.c_str(), "%d/%d %d/%d %d/%d", &v0, &uv0, &v1, &uv1, &v2, &uv2) &&
          6 != sscanf(data.c_str(), "%d//%d %d//%d %d//%d", &v0, &vn0, &v1, &vn1, &v2, &vn2) &&
          9 != sscanf(data.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", &v0, &uv0, &vn0, &v1, &uv1, &vn1, &v2, &uv2, &vn2))
      {
        std::cout << "Corrupt OBJ file face '" << data << "'" << std::endl;
        return nullptr;
      }

      if(v0 < 0) v0 = verts.size() + v0 + 1;
      if(v1 < 0) v1 = verts.size() + v1 + 1;
      if(v2 < 0) v2 = verts.size() + v2 + 1;

      triangles.push_back(Triangle(v1 - 1, v0 - 1, v2 - 1));
      materialIndicies.push_back(curMaterialIndex);
    }
    else if (line.substr(0, 6).compare("mtllib") == 0)
    {
      LoadMTLLib("./resources/meshes/" + data, materials, materialNames);
    }
    else if (line.substr(0, 6).compare("usemtl") == 0)
    {
      auto it = std::find_if(materialNames.begin(), materialNames.end(), [&](std::string name) {
        return name.compare(data) == 0;
      });

      if (it == materialNames.end())
      {
        std::cout << "Attempted to use missing material" << std::endl;
        curMaterialIndex = 0;
      }

      curMaterialIndex = it - materialNames.begin();
    }

    line.clear();
  }

  shared_ptr<Mesh> meshptr =  shared_ptr<Mesh>(new Mesh(verts, triangles));
  meshptr->SetMaterials(materials, materialIndicies);
  return meshptr;
}

namespace
{
  void AddQuad(int v1, int v2, int v3, int v4, vector<Triangle>& triangles)
  {
    triangles.push_back(Triangle(v1, v3, v2));
    triangles.push_back(Triangle(v2, v3, v4));
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
	triangles.push_back( Triangle( C, B, A) );
	triangles.push_back( Triangle( C, D, B) );

	// Left wall
	triangles.push_back( Triangle( A, E, C) );
	triangles.push_back( Triangle( C, E, G) );

	// Right wall
	triangles.push_back( Triangle( F, B, D) );
	triangles.push_back( Triangle( H, F, D) );

	// Ceiling
	triangles.push_back( Triangle( E, F, G) );
	triangles.push_back( Triangle( F, H, G) );

	// Back wall
	triangles.push_back( Triangle( G, D, C) );
	triangles.push_back( Triangle( G, H, D) );

	for (Vertex& vert : verts)
	{
		vert.position *= 2 / L;
		vert.position -= vec3(1, 1, 1);

		vert.position.x *= -1;
		vert.position.y *= -1;
	}

  return shared_ptr<Mesh>(new Mesh(verts, triangles));
}

shared_ptr<Mesh> MeshFactory::GetCube(const glm::vec3& pos /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, const glm::vec3& scale /*= glm::vec3(1.0f, 1.0f, 1.0f)*/)
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
  AddQuad(0, 1, 4, 5, triangles);
  AddQuad(3, 2, 7, 6, triangles);

  // sides
  AddQuad(1, 0, 3, 2, triangles);
  AddQuad(4, 5, 6, 7, triangles);
  AddQuad(2, 0, 6, 4, triangles);
  AddQuad(1, 3, 5, 7, triangles);

  for (Vertex& vert : verts)
  {
	  vert.position *= scale;
	  vert.position += pos;
  }

  return shared_ptr<Mesh>(new Mesh(verts, triangles));
}

std::shared_ptr<Mesh> MeshFactory::GetPlane(bool twoSided /*= false*/)
{
  const float min = -0.5f;
  const float max = 0.5f;

  vector<Vertex> verts;
  verts.reserve(4);

  verts.push_back(Vertex(vec3(min, min, 0.0f)));
  verts.push_back(Vertex(vec3(max, min, 0.0f)));
  verts.push_back(Vertex(vec3(min, max, 0.0f)));
  verts.push_back(Vertex(vec3(max, max, 0.0f)));

  vector<Triangle> triangles;
  triangles.reserve(2);

  AddQuad(1, 0, 3, 2, triangles);

  if(twoSided)
    AddQuad(0, 1, 2, 3, triangles);

  return shared_ptr<Mesh>(new Mesh(verts, triangles));
}
