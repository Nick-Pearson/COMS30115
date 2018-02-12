#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string& texturePath)
{
	std::string fullTexturePath = "./resources/textures/" + texturePath;

	imageData = stbi_load(fullTexturePath.c_str(), &width, &height, &channels, 0);
}

Texture::~Texture()
{
	stbi_image_free(imageData);
}

vec4 Texture::SampleTexture(vec2 UV) const
{
	UV.x -= floor(UV.x);
	UV.y -= floor(UV.y);

	int pixelX = (int)(UV.x * width);
	int pixelY = (int)(UV.y * height);

	vec4 val(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < channels; ++i)
	{
		val[i] = (float)imageData[i + ((pixelX + (height * pixelY)) * channels)];
	}

	return val / 255.0f;
}
