#ifndef RENDERER_H
#define RENDERER_H

#include "../TestModel.h"

#include "../SDLauxiliary.h"

class Renderer
{
public:
  Renderer(int ScreenWidth, int ScreenHeight);

  virtual void Initialise();

  void Clear();
  virtual void Draw(const std::vector<TestTriangle>& mesh) = 0;
  void SwapBuffers();

protected:

  screen* screenptr = nullptr;
};

#endif
