#include "renderer.h"

void Renderer::Initialise(int ScreenWidth, int ScreenHeight, Camera* inCamera)
{
  screenptr = InitializeSDL( ScreenWidth, ScreenHeight, false );
  camera = inCamera;
}

void Renderer::Clear()
{
  memset(screenptr->buffer, 0, screenptr->height*screenptr->width*sizeof(uint32_t));
}

void Renderer::SwapBuffers()
{
  SDL_Renderframe(screenptr);
}
