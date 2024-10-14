#pragma once

#include "Core.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::OBJECT::DATA {
	struct Sfx : Data {
		dvec1 attenuation;
		dvec1 volume;

		Sfx();
	};
}