#include <stdio.h>
#include <stdint.h>
#include <random>

#include <glm/glm.hpp>
#include "SDLauxiliary.h"

#if RAYTRACER
#include "renderer/raytracerenderer.h"
#else
#include "renderer/rasterizerenderer.h"
#endif

#include "scene/scene.h"
#include "mesh/meshfactory.h"

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720
#define SINGLE_FRAME 0

void Update(float deltaMilliseconds);

int main(int argc, char** argv)
{
  Scene* scene = new Scene;
  scene->AddMesh(MeshFactory::GetCornelRoom());
  scene->AddMesh(MeshFactory::GetCube(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.4f, 0.4f, 0.3f), glm::vec3(0.5f, 1.2f, 0.5f)));
  scene->AddMesh(MeshFactory::GetCube(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.4f, 0.7f, -0.3f), glm::vec3(0.5f, 0.5f, 0.5f)));

#if RAYTRACER
  Renderer* renderer = new RaytraceRenderer;
#else
  Renderer* renderer = new RasterizeRenderer;
#endif

  renderer->Initialise(SCREEN_WIDTH, SCREEN_HEIGHT);

  std::vector<TestTriangle> mesh;
  LoadTestModel(mesh);

  int t = SDL_GetTicks();
  srand(t);

  while( NoQuitMessageSDL() )
  {
    //render the scene
    renderer->Clear();
    renderer->Draw(scene);
    renderer->SwapBuffers();

    int t2 = SDL_GetTicks();
    float dt = (float)(t2 - t);
    float fps = 1000 / dt ;

    // update gameplay code
    std::cout << "FPS: " << fps << " last frame time " << dt << " ms" << std::endl;
    scene->Update(dt / 1000.0f);

    t = t2;

    #if SINGLE_FRAME
    break;
    #endif
  }

  renderer->SaveCurrentFrame("./render.bmp");
  delete renderer;
  delete scene;

  return 0;
}
