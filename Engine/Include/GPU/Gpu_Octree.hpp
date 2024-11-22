#pragma once

#include "Shared.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace KL::CPU {
	struct Octree;
}
namespace KL::GPU {
	struct Octree;
}

// DECL
namespace KL::CPU {
	struct Octree {
		vector<Octree> children;
		Octree* parent;
		vec3 p_min;
		vec3 p_max;
		uint item_count;
		bool discard;

		Octree();
		void growToInclude(const vec3& min, const vec3& max);

		static Octree build();
	};
}

namespace KL::GPU {
	struct alignas(16) Octree {
		vec3  p_min;
		uint  pointer_start;
		vec3  p_max;
		uint  pointer_count;
		ivec4 pointers_a;
		ivec4 pointers_b;

		Octree();
	};
}