#pragma once

#include "Include.hpp"

namespace KL::OBJECT::DATA {
	enum struct Type {
		NONE,
		ATMOSPHERE,
		PRIMITIVE,
		SKELETON,
		CAMERA,
		VOLUME,
		CURVE,
		EMPTY,
		FORCE,
		GROUP,
		LIGHT,
		MESH,
		SFX,
		VFX
	};

}

namespace KL::OBJECT {
	struct Data {
		string name;
		DATA::Type type;

		Data();
	};
}