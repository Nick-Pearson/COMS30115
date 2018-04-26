#ifndef KDTREE_H
#define KDTREE_H

#include <glm/glm.hpp>

class KDNode {
public:
  Box boundingBox;
  KDNode* child1;
  KDNode* child2;
  std::vector<int> trianglesIndices;

  KDNode() {

  }

  KDNode* build(std::vector<Triangle> tris, std::vector<int> triIndices, std::vector<Box> bounds, int depth) {
    KDNode* node = new KDNode();
    node->trianglesIndices = triIndices;
    node->child1 = NULL;
    node->child2 = NULL;

    // first, deal with the base cases
    if (triIndices.size() == 0)
      return node;

    if (triIndices.size() == 1) {
      node->boundingBox = bounds[triIndices[0]];
      node->child1 = new KDNode();
      node->child2 = new KDNode();
      node->child1->trianglesIndices = std::vector<int>();
      node->child2->trianglesIndices = std::vector<int>();
      return node;
    }

    node->boundingBox = bounds[triIndices[0]];

    // find the midpoint of all triangles
    vec3 mid(0.0f, 0.0f, 0.0f);
    for (int i=0; i<triIndices.size(); i++) {
      mid += tris[triIndices[i]].mid / (float) triIndices.size();
    }

    std::vector<int> leftTrisIndices;
    std::vector<int> rightTrisIndices;
    int longestAxis = node->boundingBox.longestAxis();

    for (int i=0; i<triIndices.size(); i++) {

      if (mid[longestAxis] >= tris[triIndices[i]].mid[longestAxis])
        leftTrisIndices.push_back(triIndices[i]);
      else
        rightTrisIndices.push_back(triIndices[i]);

    }

    if (leftTrisIndices.size() == 0 && rightTrisIndices.size() > 0)
      leftTrisIndices = rightTrisIndices;

    if (rightTrisIndices.size() == 0 && leftTrisIndices.size() > 0)
      rightTrisIndices = leftTrisIndices;

    float duplicateThreshold = 0.5f;
    bool thresholdReached = HasTooManyDupes(leftTrisIndices, rightTrisIndices, duplicateThreshold);

    if (thresholdReached) {
      node->child1 = new KDNode();
      node->child2 = new KDNode();
      node->child1->trianglesIndices = std::vector<int>();
      node->child2->trianglesIndices = std::vector<int>();
      // PrintDebug(*node, 0);
      return node;
    } else {

      node->child1 = build(tris, leftTrisIndices, bounds, depth+1);
      node->child2 = build(tris, rightTrisIndices, bounds, depth+1);
      // PrintDebug(*node, 0);
      return node;
    }

  }

  void PrintDebug(KDNode node, int depthLevel) {
    for (int i=0; i<depthLevel; i++)
        printf(" ");
    if (node.trianglesIndices.size() > 0) {
      // printf("%d: %s\n", depthLevel, node.boundingBox.toStr().c_str());

      PrintDebug(*node.child1, depthLevel+1);
      PrintDebug(*node.child2, depthLevel+1);
    } else {
      printf("%d reached leaf\n", depthLevel);
    }
  }
private:
  bool HasTooManyDupes (std::vector<int> leftTris, std::vector<int> rightTris, float threshold) {
    int leftTrisDupeThreshold = leftTris.size() * threshold;
    int rightTrisDupeThreshold = rightTris.size() * threshold;
    int duplicates = 0;
    for (int i=0; i<leftTris.size(); i++) {
      for (int j=0; j<rightTris.size(); j++) {
        if (leftTris[i] == rightTris[j]) {
          duplicates++;
          if (duplicates > leftTrisDupeThreshold || duplicates > rightTrisDupeThreshold) {
            return true;
          }
        }

      }
    }
    return false;
  }



};



#endif
