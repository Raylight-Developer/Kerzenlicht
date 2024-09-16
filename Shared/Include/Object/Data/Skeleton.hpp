#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

namespace KL::OBJECT::DATA {
	namespace SKELETON {
		struct Bone {
			Bone* parent;

			dvec3 position;

			Bone();
		};
	}
	struct Skeleton : Data {
		SKELETON::Bone* root;

		Skeleton();
	};
}