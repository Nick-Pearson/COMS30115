#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string& texturePath)
{
	std::string fullTexturePath = "./resources/textures/" + texturePath;

	imageData = stbi_load(fullTexturePath.c_str(), &width, &height, &channels, 0);
}

Texture::Texture(int inWidth, int inHeight, int inChannels)
{
  channels = inChannels;
  width = inWidth;
  height = inHeight;

  const int textureBytes = sizeof(unsigned char) * width * height * channels;

  imageData = (unsigned char*)malloc(textureBytes);
  memset(imageData, 0, textureBytes);
}

Texture::~Texture()
{
  // technically should use stbi free function but that just internally calls free anyway so it makes no difference
	free(imageData);
}

void Texture::SampleTexture(vec2 UV, uint8_t (&bytes)[4]) const
{
	UV.x -= floor(UV.x);
	UV.y -= floor(UV.y);

	int pixelX = (int)(UV.x * width) % width;
	int pixelY = (int)(UV.y * height) % height;

	bytes[0] = 0;
	bytes[1] = 0;
	bytes[2] = 0;
	bytes[3] = 255;

	for (int i = 0; i < channels; ++i)
	{
		bytes[i] = (uint8_t)imageData[i + ((pixelX + (height * pixelY)) * channels)];
	}
}

void Texture::GetPixel(int x, int y, uint8_t (&bytes)[4]) const
{
	for(int i = 0; i < channels; ++i)
	{
		bytes[i] = (uint8_t)imageData[i + ((x + (height * y)) * channels)];
	}
}

void Texture::SetPixel(int x, int y, const uint8_t (&bytes)[4])
{
	for(int i = 0; i < channels; ++i)
	{
		imageData[i + ((x + (height * y)) * channels)] = bytes[i];
	}
}



DepthTextureRenderTarget::DepthTextureRenderTarget(std::shared_ptr<Texture> inTexture)
{
	texture = inTexture;
	width = inTexture->GetWidth();
	height = inTexture->GetHeight();
}

void DepthTextureRenderTarget::PutDepth(int x, int y, float depth)
{
	u_flt_byte u;
	u.val = depth;

	texture->SetPixel(x, y, u.bytes);
}

float DepthTextureRenderTarget::GetDepth(int x, int y) const
{
	u_flt_byte u;

	texture->GetPixel(x,y, u.bytes);

	return u.val;
}
