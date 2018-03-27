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

void SDL_SaveImage(Screen *s, const char* filename)
{
  int filesize = 54 + (3*s->width*s->height);

  // https://web.archive.org/web/20080912171714/http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
  unsigned char fileHeader[14] = {
    'B', 'M', //Bitmap identifier
    (unsigned char)filesize, (unsigned char)(filesize>>8), (unsigned char)(filesize>>16), (unsigned char)(filesize>>24), //filesize
    0,0,0,0, //reserved
    54,0,0,0 //byte offset to the start of image data
  };
  unsigned char infoHeader[40] = {
    40,0,0,0, // bmpinfoheader size in bytes
    (unsigned char)s->width,(unsigned char)(s->width>>8),(unsigned char)(s->width>>16),(unsigned char)(s->width>>24), //image width
    (unsigned char)s->height,(unsigned char)(s->height>>8),(unsigned char)(s->height>>16),(unsigned char)(s->height>>24), //image height
    1, 0, //planes
    24,0 // bits per pixel (8bpc)
    // ... remainder of bits stay defauled to 0
    };
  unsigned char padding[4] = {0,0,0,0};

  FILE* fp = fopen(filename,"wb");

  if(fp == NULL) return;

  fwrite(&fileHeader, 1, 14, fp);
  fwrite(&infoHeader, 1, 40, fp);

  for(int i=0; i < s->height; i++)
  {
    const int y = s->height - i - 1;
    for(int x = 0; x < s->width; ++x)
    {
      fwrite((uint8_t*)&s->buffer[(s->width * y) + x], 1, 3, fp);
    }

    fwrite(padding, 1, (4 - (s->width * 3) % 4) % 4, fp);
  }

  fclose(fp);
}
