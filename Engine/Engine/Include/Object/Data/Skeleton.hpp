#pragma once

#include "Include.hpp"

namespace CLASS::OBJECT::DATA {
	namespace SKELETON {
		struct Bone {
			Bone* parent;

			dvec3 position;

			Bone();
		};
	}
	struct Skeleton {
		SKELETON::Bone* root;

		Skeleton();
	};
}