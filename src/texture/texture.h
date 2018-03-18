#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <glm/glm.hpp>

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

	vec4 SampleTexture(vec2 UV) const;

	inline int GetChannels() const { return channels; }
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

private:
	int width = 0;
	int height = 0;
	int channels = 0;
	
	unsigned char* imageData = nullptr;

};
#endif
