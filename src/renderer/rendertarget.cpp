#include "rendertarget.h"

#include <iostream>
#include <stdint.h>

#include "../amath.h"

void RenderTarget::SaveImage(const char* filename, bool saveDepth /*= false*/) const
{
  int filesize = 54 + (3*width*height);

  // https://web.archive.org/web/20080912171714/http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
  unsigned char fileHeader[14] = {
    'B', 'M', //Bitmap identifier
    (unsigned char)filesize, (unsigned char)(filesize>>8), (unsigned char)(filesize>>16), (unsigned char)(filesize>>24), //filesize
    0,0,0,0, //reserved
    54,0,0,0 //byte offset to the start of image data
  };
  unsigned char infoHeader[40] = {
    40,0,0,0, // bmpinfoheader size in bytes
    (unsigned char)width,(unsigned char)(width>>8),(unsigned char)(width>>16),(unsigned char)(width>>24), //image width
    (unsigned char)height,(unsigned char)(height>>8),(unsigned char)(height>>16),(unsigned char)(height>>24), //image height
    1, 0, //planes
    24,0 // bits per pixel (8bpc)
    // ... remainder of bits stay defauled to 0
    };
  unsigned char padding[4] = {0,0,0,0};

  FILE* fp = fopen(filename,"wb");

  if(fp == NULL) return;

  fwrite(&fileHeader, 1, 14, fp);
  fwrite(&infoHeader, 1, 40, fp);

  for(int i=0; i < height; i++)
  {
    const int y = height - i - 1;
    for(int x = 0; x < width; ++x)
    {
      uint8_t pixel[3];
      if(saveDepth)
      {
        uint8_t depth = (uint8_t)(100.0f * AMath::clamp(GetDepth(x, y), 0.0f, 2.55f));
        pixel[0] = depth;
        pixel[1] = depth;
        pixel[2] = depth;
      }
      else
      {
        GetPixel(x, y, pixel);
        //have to swap as .bmp is BGR rather than RGB
        std::swap(pixel[0], pixel[2]);
      }

      fwrite(&pixel, 1, 3, fp);
    }

    fwrite(padding, 1, (4 - (width * 3) % 4) % 4, fp);
  }

  fclose(fp);
}
