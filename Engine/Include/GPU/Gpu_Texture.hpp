#pragma once

#include "Shared.hpp"

#include "Shader/Shader.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	namespace GPU {
		struct alignas(16) Texture {
			uint start;
			uint width;
			uint height;
			uint format;
			// 16

			Texture(const uint& start, const uint& width, const uint& height, const SHADER::TEXTURE::Format& format, const SHADER::TEXTURE::Filtering& filtering);
			Texture(const SHADER::Texture& texture, const uint& start);
		};
	}
}