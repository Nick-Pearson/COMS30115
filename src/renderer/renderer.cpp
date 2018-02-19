#include "renderer.h"

void Renderer::Initialise(int ScreenWidth, int ScreenHeight)
{
  screenptr = InitializeSDL( ScreenWidth, ScreenHeight, false );
}

void Renderer::Clear()
{
  memset(screenptr->buffer, 0, screenptr->height*screenptr->width*sizeof(uint32_t));
  memset(screenptr->screenBuffer, 0, screenptr->height*screenptr->width * sizeof(glm::vec3));
}

void Renderer::SwapBuffers()
{
  SDL_Renderframe(screenptr);
}

void Renderer::SaveCurrentFrame(const char* filename)
{
  SDL_SaveImage(screenptr, filename);
}
