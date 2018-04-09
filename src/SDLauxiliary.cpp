#include "SDLauxiliary.h"

#include <cstring>

#include <omp.h>

void Screen::PutPixel(int x, int y, const glm::vec3& colour)
{
  uint32_t r = uint32_t(colour.r);
  uint32_t g = uint32_t(colour.g);
  uint32_t b = uint32_t(colour.b);

#pragma omp critical
  buffer[y*width + x] = (255 << 24) + (r << 16) + (g << 8) + b;
}

void Screen::GetPixel(int x, int y, uint8_t (&outColour)[3]) const
{
  outColour[0] = (uint8_t)((buffer[y*width + x] & 0x00ff0000) >> 16);
  outColour[1] = (uint8_t)((buffer[y*width + x] & 0x0000ff00) >> 8);
  outColour[2] = (uint8_t)( buffer[y*width + x] & 0x000000ff);
}

void Screen::PutFloatPixel(int x, int y, const glm::vec3& colour)
{
  uint32_t r = uint32_t(glm::clamp(255 * colour.r, 0.f, 255.f));
  uint32_t g = uint32_t(glm::clamp(255 * colour.g, 0.f, 255.f));
  uint32_t b = uint32_t(glm::clamp(255 * colour.b, 0.f, 255.f));

#pragma omp critical
  {
    floatBuffer[y*width + x].x = r;
    floatBuffer[y*width + x].y = g;
    floatBuffer[y*width + x].z = b;
  }
}

void Screen::PutDepth(int x, int y, float depth)
{
#pragma omp critical
  floatBuffer[y*width + x].w = depth;
}


Screen* InitializeSDL(int width,int height, bool fullscreen)
{
#if USE_SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) !=0)
  {
    std::cout << "Could not initialise SDL: "
	<< SDL_GetError() << std::endl;
    exit(1);
  }
#endif

  Screen *s = new Screen;
  s->width = width;
  s->height = height;
  s->buffer = new uint32_t[width*height];
  memset(s->buffer, 0, width*height*sizeof(uint32_t));

#if USE_SDL
  uint32_t flags = SDL_WINDOW_OPENGL;
  if(fullscreen)
  {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

#if RAYTRACER
  const char* WindowName = "COMS30115 - Raytracer";
#else
  const char* WindowName = "COMS30115 - Rasterizer";
#endif

  s->window = SDL_CreateWindow(WindowName,
				      SDL_WINDOWPOS_UNDEFINED,
				      SDL_WINDOWPOS_UNDEFINED,
				      width, height,flags);
  if(s->window == 0)
  {
    std::cout << "Could not set video mode: "
     << SDL_GetError() << std::endl;
    exit(1);
  }

  flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  s->renderer = SDL_CreateRenderer(s->window, -1, flags);
  if(s->renderer == 0)
  {
    std::cout << "Could not create renderer: "
     << SDL_GetError() << std::endl;
    exit(1);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(s->renderer, width,height);

  s->texture = SDL_CreateTexture(s->renderer,
				 SDL_PIXELFORMAT_ARGB8888,
				 SDL_TEXTUREACCESS_STATIC,
				 width,height);
  if(s->texture==0)
  {
    std::cout << "Could not allocate texture: "
     << SDL_GetError() << std::endl;
    exit(1);
  }
#endif

  s->floatBuffer = new glm::vec4[width*height];
  memset(s->floatBuffer, 0, width*height*sizeof(glm::vec4));

  return s;
}

bool NoQuitMessageSDL()
{
#if USE_SDL
  SDL_Event e;
  while( SDL_PollEvent(&e) )
    {
      if( e.type == SDL_QUIT )
	{
	  return false;
	}
      if( e.type == SDL_KEYDOWN )
	{
	  if( e.key.keysym.sym == SDLK_ESCAPE)
	    {
	      return false;
	    }
	}
    }
  return true;
#else
  return true;
#endif
}

void SDL_Renderframe(Screen* s)
{
#if USE_SDL
  SDL_UpdateTexture(s->texture, NULL, s->buffer, s->width * sizeof(uint32_t));
  SDL_RenderClear(s->renderer);
  SDL_RenderCopy(s->renderer, s->texture, NULL, NULL);
  SDL_RenderPresent(s->renderer);
#endif
}

void KillSDL(Screen* s)
{
  delete[] s->buffer;
  delete[] s->floatBuffer;

#if USE_SDL
  SDL_DestroyTexture(s->texture);
  SDL_DestroyRenderer(s->renderer);
  SDL_DestroyWindow(s->window);
  SDL_Quit();
#endif
}
