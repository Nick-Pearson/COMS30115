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
#include "material/phongmaterial.h"
#include "surface/sphere.h"

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

  std::shared_ptr<Material> mirrorMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 50.0f));
  std::shared_ptr<Material> frostyMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.6f, 0.0f, 50.0f));

  std::shared_ptr<Sphere> sphere = std::shared_ptr<Sphere>(new Sphere(glm::vec3(-0.5f, 0.7f, -0.4f), 0.3f, frostyMat));
  scene->AddSurface(sphere);

#if RAYTRACER
  scene->AddLight(std::shared_ptr<Light>(new PointLight(glm::vec3(1.0f, 1.0f, 1.0f), 14.0f, true, glm::vec3(0, -0.5, -0.7))));
  std::shared_ptr<Mesh> LightPlane = MeshFactory::GetCube();
  LightPlane->Translate(glm::vec3(0.0f, -1.0f, 0.0f));
  LightPlane->Scale(glm::vec3(2.0f, 0.01f, 2.0f));

  std::shared_ptr<Material> LightMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 14.0f));
  LightPlane->SetMaterial(LightMat);

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

#if !USE_SDL
    break;
#endif

    #if SINGLE_FRAME
    while (NoQuitMessageSDL()) {}
    break;
    #endif
  }

  renderer->SaveCurrentFrame("./render.bmp");
  delete renderer;
  delete scene;

  return 0;
}
