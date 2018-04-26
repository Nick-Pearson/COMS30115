#ifndef KDTREE_H
#define KDTREE_H

#include <glm/glm.hpp>

#include <vector>

#include "box.h"

class Mesh;

class KDNode {
public:
  Box boundingBox;
  KDNode* child1;
  KDNode* child2;
  std::vector<int> trianglesIndices;

  KDNode() {}
  ~KDNode() {
    delete child1;
    delete child2;
  }

  void UpdateBounds(const Mesh* mesh);

  KDNode* build(const Mesh* mesh, std::vector<int> triIndices, int depth);

  void PrintDebug(KDNode node, int depthLevel);

private:

  bool HasTooManyDupes(std::vector<int> leftTris, std::vector<int> rightTris, float threshold);


};



#endif
