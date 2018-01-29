#include <memory>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Mesh;

//static functions for handling meshes
namespace MeshFactory
{
  // TODO: Loading of any mesh from a file
  shared_ptr<Mesh> LoadFromFile() { return nullptr; }

  // A set of primatives for reuse

  // returns the outer room mesh of the cornel box
  shared_ptr<Mesh> GetCornelRoom();

  // returns a cube of 1 unit dimensions
  shared_ptr<Mesh> GetCube(const glm::vec3& colour);
};
