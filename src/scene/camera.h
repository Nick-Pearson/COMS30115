#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:
  Camera(float inFOV, const glm::vec3& inPosition) :
	  FOV(inFOV), position(inPosition)
  {}

  void Update(float deltaSeconds);

  float FOV;
  glm::vec3 position;
  glm::mat4 rotationMatrix = glm::mat4();

  float nearClipPlane = 0.1f;
  float farClipPlane = 500.0f;
};
#endif
