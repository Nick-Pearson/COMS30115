#include <stdio.h>
#include <stdint.h>

#include <glm/glm.hpp>
#include "SDLauxiliary.h"

#include "TestModel.h"
#include "renderer/raytracerenderer.h"
#include "scene/camera.h"

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720

void Update(float deltaMilliseconds);

int main(int argc, char** argv)
{
  Camera* camera = new Camera(500.0f, glm::vec3(0, 0, -2.4));
  Renderer* renderer = new RaytraceRenderer;
  renderer->Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, camera);

  std::vector<TestTriangle> mesh;
  LoadTestModel(mesh);

  int t = SDL_GetTicks();

  while( NoQuitMessageSDL() )
  {
    //render the scene
    renderer->Clear();
    renderer->Draw(mesh);
    renderer->SwapBuffers();

    int t2 = SDL_GetTicks();
    float dt = (float)(t2 - t);

    // update gameplay code
    Update(dt);
    camera->Update(dt / 1000.0f);

    t = t2;
  }

  return 0;
}

void Update(float deltaMilliseconds)
{
  std::cout << "Last frame time " << deltaMilliseconds << " ms" << std::endl;
}
