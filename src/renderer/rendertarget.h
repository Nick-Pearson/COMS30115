#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <glm/glm.hpp>

// base class that represents a container for buffers - this is to enable rendering to textures primarily for the use of shadow maps
class RenderTarget
{
public:

  // set a pixel in the standard buffer (1 byte per channel)
  virtual void PutPixel(int x, int y, const glm::vec3& colour) = 0;

  //get a value from the standard buffer
  virtual void GetPixel(int x, int y, uint8_t (&outColour)[3]) const= 0;

  // set a pixel in the HDR buffer (1 floating point value per channel)
  virtual void PutFloatPixel(int x, int y, const glm::vec3& colour) = 0;

  // set a value in the depth buffer (floating point value)
  virtual void PutDepth(int x, int y, float depth) = 0;

  // get a value from the depth buffer
  virtual float GetDepth(int x, int y) const = 0;

public:

  void SaveImage(const char* filename, bool saveDepth = false) const;

public:
  int height;
  int width;
};

#endif
