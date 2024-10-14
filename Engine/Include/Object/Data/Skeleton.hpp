#pragma once

#include "Core.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
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