#ifndef RENDERER_H
#define RENDERER_H

#include "../TestModel.h"

#include "../SDLauxiliary.h"

class Camera;

class Renderer
{
public:
  virtual void Initialise(int ScreenWidth, int ScreenHeight, Camera* inCamera);

  void Clear();
  virtual void Draw(const std::vector<TestTriangle>& mesh) = 0;
  void SwapBuffers();

protected:

  screen* screenptr = nullptr;

  Camera* camera = nullptr;
};

#endif
