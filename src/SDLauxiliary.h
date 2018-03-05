#ifndef SDL_AUXILIARY_H
#define SDL_AUXILIARY_H

#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include <stdint.h>

typedef struct{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  int height;
  int width;
  uint32_t *buffer;
  glm::vec4* floatBuffer; /* xyz contain colour info, w contains invdepth */
} screen;

screen* InitializeSDL( int width, int height, bool fullscreen = false );
bool NoQuitMessageSDL();
void PutPixelSDL( screen *s, int x, int y, glm::vec3 color );
void PutFloatPixelSDL(screen* s, int x, int y, glm::vec3 colour);
void PutDepthSDL(screen* s, int x, int y, float depth);
inline float GetDepthSDL(const screen* s, int x, int y) { return s->floatBuffer[y*s->width + x].w; }
void SDL_Renderframe(screen *s);
void KillSDL(screen* s);
void SDL_SaveImage(screen *s, const char* filename);

#endif
