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
#include "texture/texture.h"

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720
#define SINGLE_FRAME 0

void Update(float deltaMilliseconds);

int main(int argc, char** argv)
{
  Scene* scene = new Scene;


  std::shared_ptr<Material> floorMat(new PhongMaterial(glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.6f, 0.6f, 0.6f), 0.0f, 0.0f, 10.0f));
  floorMat->albedoTexture = std::shared_ptr<Texture>(new Texture("floor/ft_broken01_c.png"));
  floorMat->normalTexture = std::shared_ptr<Texture>(new Texture("floor/ft_broken01_n.png"));

  std::shared_ptr<Mesh> Cornel = MeshFactory::LoadFromFile("room.obj");
  Cornel->SetMaterialOnTriangle(floorMat, 6);
  Cornel->SetMaterialOnTriangle(floorMat, 7);
  scene->AddMesh(Cornel);

  scene->AddMesh(MeshFactory::LoadFromFile("box1.obj"));
  scene->AddMesh(MeshFactory::LoadFromFile("box2.obj"));

  std::shared_ptr<Mesh> Bunny = MeshFactory::LoadFromFile("bunny.obj");
  Bunny->Scale(2.5f);
  Bunny->Translate(glm::vec3(0.4f, 0.1f, -0.3f));
  Bunny->Rotate(glm::vec3(0.0f, 180.0f, 180.0f));
  scene->AddMesh(Bunny);

  std::shared_ptr<Material> mirrorMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 50.0f));
  std::shared_ptr<Material> frostyMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.4f, 0.0f, 50.0f));
  std::shared_ptr<Material> glassMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 10.0f, 1.0f, 1.7f));

  scene->AddSurface(std::shared_ptr<Sphere>(new Sphere(glm::vec3(-0.5f, 0.7f, -0.4f), 0.3f, mirrorMat)));
  scene->AddSurface(std::shared_ptr<Sphere>(new Sphere(glm::vec3(-0.1f, 0.7f, -0.8f), 0.2f, glassMat)));
#if RAYTRACER
  scene->AddLight(std::shared_ptr<Light>(new PointLight(glm::vec3(1.0f, 1.0f, 1.0f), 12.0f, true, glm::vec3(0, -0.5, -0.7))));
  std::shared_ptr<Mesh> LightPlane = MeshFactory::GetCube();
  LightPlane->Translate(glm::vec3(0.0f, -1.0f, 0.0f));
  LightPlane->Scale(glm::vec3(2.0f, 0.01f, 2.0f));

  std::shared_ptr<Material> LightMat(new PhongMaterial(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 13.0f));
  LightPlane->SetMaterial(LightMat);

  scene->AddMesh(LightPlane);

  Renderer* renderer = new RaytraceRenderer;
#else // RASTERIZER
  scene->AddLight(std::shared_ptr<Light>(new PointLight(glm::vec3(1.0f, 1.0f, 1.0f), 60.0f, true, glm::vec3(0, 0, -2.4))));

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
