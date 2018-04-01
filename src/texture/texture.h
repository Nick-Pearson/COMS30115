#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "../renderer/rendertarget.h"

using glm::vec4;
using glm::vec3;
using glm::vec2;

class Texture
{
public:

	// loads texture from the specified path relative to the resources directory
	Texture(const std::string& texturePath);

  // creates an empty texture of the desired size
  Texture(int inWidth, int inHeight, int inChannels = 3);

	virtual ~Texture();

	void SampleTexture(vec2 UV, uint8_t (&bytes)[4]) const;

	void GetPixel(int x, int y, uint8_t (&bytes)[4]) const;
	void SetPixel(int x, int y, const uint8_t (&bytes)[4]);

	inline int GetChannels() const { return channels; }
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

private:
	int width = 0;
	int height = 0;
	int channels = 0;

	unsigned char* imageData = nullptr;
};

class DepthTextureRenderTarget : public RenderTarget
{
	//hacky way we can store a float as 4 bytes which fits in a 4 channel texture
	union u_flt_byte
	{
		float val;
		uint8_t bytes[4];
	};

public:

	DepthTextureRenderTarget(std::shared_ptr<Texture> inTexture);

  inline void PutPixel(int x, int y, const glm::vec3& color) override {}
  inline void PutFloatPixel(int x, int y, const glm::vec3& colour) override {}
  inline void GetPixel(int x, int y, uint8_t (&outColour)[3]) const override {}

  void PutDepth(int x, int y, float depth) override;
  float GetDepth(int x, int y) const override;

public:

	std::shared_ptr<Texture> texture;
};
#endif
