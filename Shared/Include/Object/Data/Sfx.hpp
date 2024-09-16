#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

namespace KL::OBJECT::DATA {
	struct Sfx : Data {
		dvec1 attenuation;
		dvec1 volume;

		Sfx();
	};
}