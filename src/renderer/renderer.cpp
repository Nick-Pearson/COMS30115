#include "renderer.h"

#include <cstring>

void Renderer::Initialise(int ScreenWidth, int ScreenHeight)
{
  screenptr = InitializeSDL( ScreenWidth, ScreenHeight, false );
}

void Renderer::Clear()
{
  std::memset(screenptr->buffer, 0, screenptr->height * screenptr->width * sizeof(uint32_t));
  std::memset(screenptr->floatBuffer, 0, screenptr->height*screenptr->width * sizeof(glm::vec4));
}

void Renderer::SwapBuffers()
{
  SDL_Renderframe(screenptr);
}

void Renderer::SaveCurrentFrame(const char* filename)
{
  screenptr->SaveImage(filename);
}
