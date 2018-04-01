#include "texturecubemap.h"

TextureCubemap::TextureCubemap(const std::string& textureDirectory)
{
	Images[(int)CubemapDirections::Top] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/posy.jpg"));
	Images[(int)CubemapDirections::Bottom] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/negy.jpg"));
	Images[(int)CubemapDirections::Front] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/posx.jpg"));
	Images[(int)CubemapDirections::Back] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/negx.jpg"));
	Images[(int)CubemapDirections::Left] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/negz.jpg"));
	Images[(int)CubemapDirections::Right] = std::shared_ptr<Texture>(new Texture(textureDirectory + "/posz.jpg"));
}


TextureCubemap::TextureCubemap(std::shared_ptr<Texture> topTexture, std::shared_ptr<Texture> bottomTexture, std::shared_ptr<Texture> frontTexture, std::shared_ptr<Texture> backTexture, std::shared_ptr<Texture> leftTexture, std::shared_ptr<Texture> rightTexture)
{
	Images[(int)CubemapDirections::Top] = topTexture;
	Images[(int)CubemapDirections::Bottom] = bottomTexture;
	Images[(int)CubemapDirections::Front] = frontTexture;
	Images[(int)CubemapDirections::Back] = backTexture;
	Images[(int)CubemapDirections::Left] = leftTexture;
	Images[(int)CubemapDirections::Right] = rightTexture;
}


TextureCubemap::TextureCubemap(int inWidth, int inHeight, int inChannels /*= 3*/)
{
	for(int i = 0; i < 6; ++i)
	{
		Images[i] = std::shared_ptr<Texture>(new Texture(inWidth, inHeight, inChannels));
	}
}

void TextureCubemap::GetCubemapColourWithDepth(const vec3& direction, uint8_t (&colour)[4], float* outDepth /*= nullptr*/) const
{
	CubemapDirections Dir = CubemapDirections::Top;
	vec3 absoluteDirection = vec3(abs(direction.x), abs(direction.y), abs(direction.z));
	vec2 UV(0.5f, 0.5f);

	if(outDepth) *outDepth = 0.0f;

	if (absoluteDirection.y >= absoluteDirection.x && absoluteDirection.y >= absoluteDirection.z && absoluteDirection.y != 0.0f)
	{
		Dir = direction.y > 0.0f ? CubemapDirections::Bottom : CubemapDirections::Top;
		UV.x = -0.5f * ((direction.z / direction.y) + 1.0f);
		UV.y = -0.5f * ((direction.x / absoluteDirection.y) + 1.0f);
		if(outDepth) *outDepth = absoluteDirection.y;
	}
	else if (absoluteDirection.z >= absoluteDirection.x && absoluteDirection.z >= absoluteDirection.y && absoluteDirection.z != 0.0f)
	{
		Dir = direction.z > 0.0f ? CubemapDirections::Front : CubemapDirections::Back;
		UV.x = 0.5f * ((direction.x / direction.z) + 1.0f);
		UV.y = 0.5f * ((direction.y / absoluteDirection.z) + 1.0f);
		if(outDepth) *outDepth = absoluteDirection.z;
	}
	else if (absoluteDirection.x >= absoluteDirection.z && absoluteDirection.x >= absoluteDirection.z && absoluteDirection.x != 0.0f)
	{
		Dir = direction.x > 0.0f ? CubemapDirections::Left : CubemapDirections::Right;
		UV.x = -0.5f * ((direction.z / direction.x) + 1.0f);
		UV.y = 0.5f * ((direction.y / absoluteDirection.x) + 1.0f);
		if(outDepth) *outDepth = absoluteDirection.x;
	}

	Images[(int)Dir]->SampleTexture(UV, colour);
}
