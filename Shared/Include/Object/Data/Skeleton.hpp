#pragma once

#include "Include.hpp"

namespace KL::OBJECT::DATA {
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