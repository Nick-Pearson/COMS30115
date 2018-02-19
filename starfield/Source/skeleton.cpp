#include <iostream>
#include <glm/glm.hpp>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <stdint.h>

using namespace std;
using glm::vec3;
using glm::mat3;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 256
#define FULLSCREEN_MODE false
int t;
vector<vec3> stars( 1000 );

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw(screen* screen, const vector<vec3>& stars);

template<typename T>
void Interpolate(T a, T b, std::vector<T>& result);

int main( int argc, char* argv[] )
{
  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );
  t = SDL_GetTicks(); /*Set start value for timer.*/



  for(int i = 0; i < stars.size(); ++i)
  {
    vec3& pos = stars[i];
    pos.x = (float(rand()) * 2.0f / float(RAND_MAX)) - 1.0f;
    pos.y = (float(rand()) * 2.0f / float(RAND_MAX)) - 1.0f;
    pos.z = float(rand()) / float(RAND_MAX);
  }

  while( NoQuitMessageSDL() )
  {
    Draw(screen, stars);
    Update();
    SDL_Renderframe(screen);
  }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen, const vector<vec3>& stars)
{
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  float focalLength = screen->height * 0.5f;


  for(size_t s=0; s<stars.size(); ++s)
  {
    vec3 color = 0.2f * vec3(1,1,1) / (stars[s].z*stars[s].z);
    const vec3& star = stars[s];
    float u = (focalLength * (star.x / star.z)) + (screen->width * 0.5f);
    float v = (focalLength * (star.y / star.z)) + (screen->height * 0.5f);

    if(u < 0.0f || v < 0.0f || u > screen->width || v > screen->height)
      continue;

    PutPixelSDL(screen, (int)u, (int)v, color);
  }


  /*
  COLOUR INTERPOLATION

  vec3 topLeft(1.0f, 0.0f, 0.0f);
  vec3 topRight(0.0f, 0.0f, 1.0f);
  vec3 bottomLeft(1.0f, 1.0f, 0.0f);
  vec3 bottomRight(0.0f, 1.0f, 0.0f);

  vector<vec3> leftSide(screen->height);
  vector<vec3> rightSide(screen->height);
  Interpolate(topLeft, bottomLeft, leftSide);
  Interpolate(topRight, bottomRight, rightSide);

  for(int y = 0; y < screen->height; ++y)
  {
    vector<vec3> rowVals(screen->width);
    Interpolate(leftSide[y], rightSide[y], rowVals);

    for(int x = 0; x < screen->width; ++x)
    {
      PutPixelSDL(screen, x, y, rowVals[x]);
    }
  }*/
}

/*Place updates of parameters here*/
void Update()
{

  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  /*Good idea to remove this*/

  t = t2;

  float v = 0.0005;

  for( int s=0; s<stars.size(); ++s )
  {

    stars[s].z -= v * dt;

    if( stars[s].z <= 0 )
    stars[s].z += 1;
    if( stars[s].z > 1 )
    stars[s].z -= 1;
  }


}
