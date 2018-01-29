#include <stdio.h>
#include <stdint.h>

#include <glm/glm.hpp>
#include <SDL.h>

#include "TestModel.h"
#include "renderer/raytracerenderer.h"

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720

int main(int argc, char** argv)
{
  Renderer* renderer = new RaytraceRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

  std::vector<TestTriangle> mesh;
  LoadTestModel(mesh);

  while( NoQuitMessageSDL() )
  {
    //render the scene
    renderer->Clear();
    renderer->Draw(mesh);
    renderer->SwapBuffers();

    // update gameplay code
    //Update();
  }

  return 0;
}
