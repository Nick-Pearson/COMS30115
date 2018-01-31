#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:
  Camera(float inFocalLength, const glm::vec3& inPosition) :
    focalLength(inFocalLength), position(inPosition)
  {}

  void Update(float deltaSeconds);

  float focalLength;
  glm::vec3 position;
  glm::mat4 rotationMatrix = glm::mat4();
};
#endif
