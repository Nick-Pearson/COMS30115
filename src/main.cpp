#include <stdio.h>
#include <stdint.h>
#include <random>
#include <memory>

#include <glm/glm.hpp>
#include "SDLauxiliary.h"

#include "renderer/raytracerenderer.h"
#include "renderer/rasterizerenderer.h"

#include "scene/scene.h"
#include "mesh/meshfactory.h"
#include "light/pointlight.h"
#include "mesh/mesh.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SINGLE_FRAME 0

void Update(float deltaMilliseconds);

int main(int argc, char** argv)
{
  Scene* scene = new Scene;
  scene->AddMesh(MeshFactory::GetCornelRoom());
  scene->AddMesh(MeshFactory::GetCube(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.4f, 0.4f, 0.3f), glm::vec3(0.5f, 1.2f, 0.5f)));
  scene->AddMesh(MeshFactory::GetCube(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.4f, 0.7f, -0.3f), glm::vec3(0.5f, 0.5f, 0.5f)));

  std::shared_ptr<Mesh> Bunny = MeshFactory::LoadFromFile("bunny.obj");
  Bunny->Scale(2.5f);
  Bunny->Translate(glm::vec3(0.5f, 0.15f, -0.3f));
  Bunny->Rotate(glm::vec3(0.0f, 180.0f, 180.0f));
  Bunny->FlipNormals();
  scene->AddMesh(Bunny);

  scene->AddLight(std::shared_ptr<Light>(new PointLight(glm::vec3(1.0f, 1.0f, 1.0f), 14.0f, true, glm::vec3(0, -0.5, -0.7))));

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
