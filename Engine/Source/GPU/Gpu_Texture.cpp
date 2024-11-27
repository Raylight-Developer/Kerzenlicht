#include "GPU/Gpu_Texture.hpp"

#include "Utils/Session.hpp"

KL::GPU::Texture::Texture(const uint& start, const uint& width, const uint& height, const SHADER::TEXTURE::Format& format, const SHADER::TEXTURE::Filtering& filtering) :
	start(start),
	width(width),
	height(height),
	format(packTwoUint16(e_to_us(format), e_to_us(filtering)))
{}

KL::GPU::Texture::Texture(const SHADER::Texture & texture, const uint& start) :
	start(start),
	width(texture.resolution.x),
	height(texture.resolution.t),
	format(packTwoUint16(e_to_us(texture.format), e_to_us(texture.filtering)))
{}