
#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include <string>
#include <memory>

#include "cubemap.h"

class Texture;

enum class CubemapDirections : uint8_t
{
	Top = 0,
	Bottom,
	Front,
	Back,
	Left,
	Right
};

class TextureCubemap : public Cubemap
{
public:

	// loads textures for the cubemap with the convention posx, posy, posz, negx, negy, negz
	TextureCubemap(const std::string& textureDirectory);

	vec3 GetCubemapColour(const vec3& direction) const override;

private:

	std::shared_ptr<Texture> Images[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

#endif

