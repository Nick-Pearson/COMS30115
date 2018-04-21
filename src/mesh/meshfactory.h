#ifndef MESHFACTORY_H
#define  MESHFACTORY_H

#include <memory>
#include <string>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Mesh;

//static functions for handling meshes
namespace MeshFactory
{
  // enumeration of all mesh filetypes
  enum MeshType
  {
    OBJ = 0,

    UNKOWN = 255
  };

  //returns the mesh from a number of supported file formats
  shared_ptr<Mesh> LoadFromFile(const std::string& filepath);

  // A set of primatives for reuse

  // returns the outer room mesh of the cornel box
  shared_ptr<Mesh> GetCornelRoom();

  // returns a cube of 1 unit dimensions
  shared_ptr<Mesh> GetCube(const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));

  //returns a plane of 1 unit dimensions
  shared_ptr<Mesh> GetPlane(bool twoSided = false);
};

#endif
