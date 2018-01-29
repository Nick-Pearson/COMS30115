#include <stdio.h>
#include <stdint.h>

#include <glm/glm.hpp>
#include "SDLauxiliary.h"

#include "TestModel.h"
#include "renderer/raytracerenderer.h"

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720

void Update(float deltaMilliseconds);

int main(int argc, char** argv)
{
  Renderer* renderer = new RaytraceRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

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
    float dt = t2 - t;

    // update gameplay code
    Update(dt);

    t = t2;
  }

  return 0;
}

void Update(float deltaMilliseconds)
{
  std::cout << "Last frame time " << deltaMilliseconds << " ms" << std::endl;
}
