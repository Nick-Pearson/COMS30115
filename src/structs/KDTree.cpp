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

KDNode* KDNode::build(const Mesh* mesh, std::vector<int> triIndices, int depth)
{
  if (!mesh) return nullptr;

  KDNode* node = new KDNode();
  node->trianglesIndices = triIndices;

  node->UpdateBounds(mesh);

  node->child1 = NULL;
  node->child2 = NULL;

  // first, deal with the base cases
  if (triIndices.size() <= 1)
    return node;

  // find the midpoint of all triangles
  std::vector<vec3> mids(triIndices.size());
  vec3 mid(0.0f, 0.0f, 0.0f);

  for (int i = 0; i<triIndices.size(); i++) {
    const glm::vec3 a0 = mesh->Verticies[mesh->Triangles[i].v0].position;
    const glm::vec3 a1 = mesh->Verticies[mesh->Triangles[i].v1].position;
    const glm::vec3 a2 = mesh->Verticies[mesh->Triangles[i].v2].position;
    mids[i] = (a0 + a1 + a2) / 3.0f;

    mid += mids[i] / (float)triIndices.size();
  }

  std::vector<int> leftTrisIndices;
  std::vector<int> rightTrisIndices;
  int longestAxis = node->boundingBox.longestAxis();

  for (int i = 0; i<triIndices.size(); i++) {

    if (mid[longestAxis] >= mids[i][longestAxis])
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

  if (thresholdReached)
    return node;

  node->child1 = build(mesh, leftTrisIndices, depth + 1);
  node->child2 = build(mesh, rightTrisIndices, depth + 1);
  node->trianglesIndices.clear();

  return node;
}

void KDNode::PrintDebug(KDNode node, int depthLevel)
{
  for (int i = 0; i<depthLevel; i++)
    printf(" ");
  if (node.trianglesIndices.size() > 0) {
    // printf("%d: %s\n", depthLevel, node.boundingBox.toStr().c_str());

    PrintDebug(*node.child1, depthLevel + 1);
    PrintDebug(*node.child2, depthLevel + 1);
  }
  else {
    printf("%d reached leaf\n", depthLevel);
  }
}

bool KDNode::HasTooManyDupes(std::vector<int> leftTris, std::vector<int> rightTris, float threshold)
{
  int leftTrisDupeThreshold = leftTris.size() * threshold;
  int rightTrisDupeThreshold = rightTris.size() * threshold;
  int duplicates = 0;
  for (int i = 0; i<leftTris.size(); i++) {
    for (int j = 0; j<rightTris.size(); j++) {
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
