#ifndef SDL_AUXILIARY_H
#define SDL_AUXILIARY_H

#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include <stdint.h>

#include "renderer/rendertarget.h"

class Screen : public RenderTarget
{
public:

  void PutPixel(int x, int y, const glm::vec3& color) override;
  void PutFloatPixel(int x, int y, const glm::vec3& colour) override;
  void PutDepth(int x, int y, float depth) override;
  inline float GetDepth(int x, int y) const override { return floatBuffer[y*width + x].w; }
  
public:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint32_t *buffer;
  glm::vec4* floatBuffer; /* xyz contain colour info, w contains invdepth */
};

Screen* InitializeSDL( int width, int height, bool fullscreen = false );
bool NoQuitMessageSDL();
void SDL_Renderframe(Screen *s);
void KillSDL(Screen* s);
void SDL_SaveImage(Screen *s, const char* filename);

#endif
