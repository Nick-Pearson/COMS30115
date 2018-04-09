
#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include <string>
#include <memory>

#include "cubemap.h"
#include "texture.h"

enum class CubemapDirections : uint8_t
{
	Top = 0,
	Bottom,
	Front,
	Back,
	Left,
	Right,

	MAX
};

class TextureCubemap : public Cubemap
{
public:

	// loads textures for the cubemap with the convention posx, posy, posz, negx, negy, negz
	TextureCubemap(const std::string& textureDirectory);

	//initialise with textures directly
	TextureCubemap(std::shared_ptr<Texture> topTexture, std::shared_ptr<Texture> bottomTexture, std::shared_ptr<Texture> frontTexture, std::shared_ptr<Texture> backTexture, std::shared_ptr<Texture> leftTexture, std::shared_ptr<Texture> rightTexture);

	TextureCubemap(int inWidth, int inHeight, int inChannels = 3);

	//samples the cubemap based on a direction, optionally outputs the projected depth of the vector on the cubemap
	void GetCubemapColour(const vec3& direction, uint8_t (&colour)[4]) const override { return GetCubemapColourWithDepth(direction, colour); }
	void GetCubemapColourWithDepth(const vec3& direction, uint8_t (&colour)[4], float* outDepth = nullptr) const;

	std::shared_ptr<Texture> Images[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

#endif
