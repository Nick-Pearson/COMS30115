#include "KDTree.h"

#include "../mesh/mesh.h"

void KDNode::UpdateBounds(const Mesh* mesh)
{
  if (child1 && child2)
  {
    child1->UpdateBounds(mesh);
    child2->UpdateBounds(mesh);

    boundingBox.Invalidate();
    boundingBox += child1->boundingBox;
    boundingBox += child2->boundingBox;
    return;
  }

  boundingBox.Invalidate();

  for (const int idx : trianglesIndices)
  {
    boundingBox += mesh->Verticies[mesh->Triangles[idx].v0].position;
    boundingBox += mesh->Verticies[mesh->Triangles[idx].v1].position;
    boundingBox += mesh->Verticies[mesh->Triangles[idx].v2].position;
  }
}

KDNode* KDNode::build(const Mesh* mesh, std::vector<int> localTriangleIndices, int depth) const
{
  if (!mesh) return nullptr;

  KDNode* node = new KDNode();
  node->trianglesIndices = localTriangleIndices;
  node->UpdateBounds(mesh);

  // First, deal with the base cases
  if (localTriangleIndices.size() <= 15)
    return node;

  // Find the midpoint of all triangles
  std::vector<vec3> mids(localTriangleIndices.size());
  vec3 mid(0.0f, 0.0f, 0.0f);

  for (int i = 0; i < localTriangleIndices.size(); i++) {
    const glm::vec3 a0 = mesh->Verticies[mesh->Triangles[i].v0].position;
    const glm::vec3 a1 = mesh->Verticies[mesh->Triangles[i].v1].position;
    const glm::vec3 a2 = mesh->Verticies[mesh->Triangles[i].v2].position;
    mids[i] = (a0 + a1 + a2) / 3.0f;
    mid += mids[i] / (float)localTriangleIndices.size();
  }

  std::vector<int> leftTriangleIndices;
  std::vector<int> rightTriangleIndices;
  int longestAxis = node->boundingBox.longestAxis();

  // Sort triangles via midpoint
  for (int i = 0; i < localTriangleIndices.size(); i++) {
    if (mid[longestAxis] >= mids[i][longestAxis]) {
      leftTriangleIndices.push_back(localTriangleIndices[i]);
    } else {
      rightTriangleIndices.push_back(localTriangleIndices[i]);
    }
  }

  if (leftTriangleIndices.size() == 0 && rightTriangleIndices.size() > 0)
    leftTriangleIndices = rightTriangleIndices;

  if (rightTriangleIndices.size() == 0 && leftTriangleIndices.size() > 0)
    rightTriangleIndices = leftTriangleIndices;

  float duplicateThreshold = 0.5f;
  bool thresholdReached = HasDuplicates(leftTriangleIndices, rightTriangleIndices, duplicateThreshold);

  if (thresholdReached)
    return node;

  node->child1 = build(mesh, leftTriangleIndices, depth + 1);
  node->child2 = build(mesh, rightTriangleIndices, depth + 1);
  node->trianglesIndices.clear();

  return node;
}

bool KDNode::HasDuplicates(std::vector<int> leftTriangleIndices, std::vector<int> rightTriangleIndices, float threshold) const
{
  int leftTrisDupeThreshold = leftTriangleIndices.size() * threshold;
  int rightTrisDupeThreshold = rightTriangleIndices.size() * threshold;
  int duplicates = 0;
  for (int i = 0; i < leftTriangleIndices.size(); i++) {
    for (int j = 0; j < rightTriangleIndices.size(); j++) {
      if (leftTriangleIndices[i] == rightTriangleIndices[j]) {
        duplicates++;
        if (duplicates > leftTrisDupeThreshold || duplicates > rightTrisDupeThreshold) {
          return true;
        }
      }

    }
  }
  return false;
}
