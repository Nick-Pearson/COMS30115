#include "camera.h"

#include <SDL.h>

#include <glm/gtx/transform.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

void Camera::Update(float deltaSeconds)
{
  const float CameraMovementSpeed = 3.0f;
  const float CameraTurnSpeed = 0.5f;

  const uint8_t* keystate = SDL_GetKeyboardState( nullptr );

  // move the camera
  glm::vec3 deltaPosition;

  if( keystate[SDL_SCANCODE_W] )
  {
    deltaPosition += vec3(0.0f, 0.0f, 1.0f) * CameraMovementSpeed * deltaSeconds;
  }
  else if( keystate[SDL_SCANCODE_S] )
  {
    deltaPosition += vec3(0.0f, 0.0f, -1.0f) * CameraMovementSpeed * deltaSeconds;
  }

  if( keystate[SDL_SCANCODE_A] )
  {
    deltaPosition += vec3(-1.0f, 0.0f, 0.0f) * CameraMovementSpeed * deltaSeconds;
  }
  else if( keystate[SDL_SCANCODE_D] )
  {
    deltaPosition += vec3(1.0f, 0.0f, 0.0f) * CameraMovementSpeed * deltaSeconds;
  }

  // Rotate the view
  if( keystate[SDL_SCANCODE_UP] )
  {
    rotationMatrix = glm::rotate(rotationMatrix, CameraTurnSpeed * deltaSeconds, vec3(1.0f, 0.0f, 0.0f));
  }
  else if( keystate[SDL_SCANCODE_DOWN] )
  {
    rotationMatrix = glm::rotate(rotationMatrix, CameraTurnSpeed * deltaSeconds, vec3(-1.0f, 0.0f, 0.0f));
  }

  if( keystate[SDL_SCANCODE_LEFT] )
  {
    rotationMatrix = glm::rotate(rotationMatrix, CameraTurnSpeed * deltaSeconds, vec3(0.0f, -1.0f, 0.0f));
  }
  else if( keystate[SDL_SCANCODE_RIGHT] )
  {
    rotationMatrix = glm::rotate(rotationMatrix, CameraTurnSpeed * deltaSeconds, vec3(0.0f, 1.0f, 0.0f));
  }

  position += vec3(rotationMatrix * vec4(deltaPosition, 1.0f));

  // reset the camera to 0,0
  if( keystate[SDL_SCANCODE_R] )
  {
    rotationMatrix = mat4();
    position = vec3(0.0f, 0.0f, 0.0f);
  }
}
