#include "texturecubemap.h"

#include "texture.h"

TextureCubemap::TextureCubemap(const std::string& textureDirectory)
{
	Images[(int)CubemapDirections::Right] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/posz.jpg"));
	Images[(int)CubemapDirections::Left] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/negz.jpg"));
	Images[(int)CubemapDirections::Front] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/posx.jpg"));
	Images[(int)CubemapDirections::Back] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/negx.jpg"));
	Images[(int)CubemapDirections::Bottom] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/negy.jpg"));
	Images[(int)CubemapDirections::Top] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/posy.jpg"));
}

vec3 TextureCubemap::GetCubemapColour(const vec3& direction) const
{
	CubemapDirections Dir = CubemapDirections::Top;
	vec3 absoluteDirection = vec3(abs(direction.x), abs(direction.y), abs(direction.z));
	vec2 UV(0.5f, 0.5f);

	if (absoluteDirection.y >= absoluteDirection.x && absoluteDirection.y >= absoluteDirection.z)
	{
		Dir = direction.y > 0.0f ? CubemapDirections::Bottom : CubemapDirections::Top;
		UV.x = 0.5f * ((direction.x / absoluteDirection.y) + 1.0f);
		UV.y = -0.5f * ((direction.z / direction.y) + 1.0f);
	}
	else if (absoluteDirection.x >= absoluteDirection.z && absoluteDirection.x >= absoluteDirection.y)
	{
		Dir = direction.x > 0.0f ? CubemapDirections::Front : CubemapDirections::Back;
		UV.x = -0.5f * ((direction.z / direction.x) + 1.0f);
		UV.y = 0.5f * ((direction.y / absoluteDirection.x) + 1.0f);
	}
	else if (absoluteDirection.z >= absoluteDirection.x && absoluteDirection.z >= absoluteDirection.y)
	{
		Dir = direction.z > 0.0f ? CubemapDirections::Right : CubemapDirections::Left;
		UV.x = 0.5f * ((direction.x / direction.z) + 1.0f);
		UV.y = 0.5f * ((direction.y / absoluteDirection.z) + 1.0f);
	}

	return vec3(Images[(int)Dir]->SampleTexture(UV));
}
