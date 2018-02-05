#ifndef RENDERER_H
#define RENDERER_H

#include "../TestModel.h"

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

protected:

  screen* screenptr = nullptr;
};

#endif
