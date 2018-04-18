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

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720
#define SINGLE_FRAME 0

void Update(float deltaMilliseconds);

int main(int argc, char** argv)
{
  Scene* scene = new Scene;

  scene->AddMesh(MeshFactory::LoadFromFile("cornel.obj"));
  //scene->AddMesh(MeshFactory::GetCornelRoom());


  std::shared_ptr<Mesh> Bunny = MeshFactory::LoadFromFile("bunny.obj");
  Bunny->Scale(2.5f);
  Bunny->Translate(glm::vec3(0.4f, 0.1f, -0.3f));
  Bunny->Rotate(glm::vec3(0.0f, 180.0f, 180.0f));
  //scene->AddMesh(Bunny);


#if RAYTRACER
  scene->AddLight(std::shared_ptr<Light>(new PointLight(glm::vec3(1.0f, 1.0f, 1.0f), 14.0f, true, glm::vec3(0, -0.5, -0.7))));
  std::shared_ptr<Mesh> LightPlane = MeshFactory::GetPlane();
  scene->AddMesh(LightPlane);

  Renderer* renderer = new RaytraceRenderer;
#else // RASTERIZER
  scene->AddLight(std::shared_ptr<Light>(new PointLight(glm::vec3(1.0f, 1.0f, 1.0f), 50.0f, true, glm::vec3(0, 0, -2.4))));

  Renderer* renderer = new RasterizeRenderer;
#endif

  renderer->Initialise(SCREEN_WIDTH, SCREEN_HEIGHT);

  #if USE_SDL
int t = SDL_GetTicks();
srand(t);
  #endif

  while( NoQuitMessageSDL() )
  {
    //render the scene
    renderer->Clear();
    renderer->Draw(scene);
    renderer->SwapBuffers();

#if USE_SDL
    int t2 = SDL_GetTicks();
    float dt = (float)(t2 - t);
    float fps = 1000 / dt ;

    // update gameplay code
    printf("FPS: %.3f last frame time %.0f ms\n", fps, dt);
    scene->Update(dt / 1000.0f);

    t = t2;
#endif

    #if (SINGLE_FRAME || !USE_SDL)
    break;
    #endif
  }

  renderer->SaveCurrentFrame("./render.bmp");
  delete renderer;
  delete scene;

  return 0;
}
