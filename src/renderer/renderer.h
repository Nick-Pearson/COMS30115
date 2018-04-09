#ifndef RENDERER_H
#define RENDERER_H

#include "../SDLauxiliary.h"

class Scene;

class Renderer
{
public:
  virtual ~Renderer() {}

  virtual void Initialise(int ScreenWidth, int ScreenHeight);

  void Clear();
  virtual void Draw(const Scene* scene) = 0;
  void SwapBuffers();

  void SaveCurrentFrame(const char* filename);

protected:

  Screen* screenptr = nullptr;
};

#endif
