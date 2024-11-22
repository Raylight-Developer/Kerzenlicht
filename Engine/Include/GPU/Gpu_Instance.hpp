#pragma once

#include "Shared.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::GPU {
	struct alignas(16) Instance {
		mat4 object_matrix;
		uint object_index;
		vec3 padding = vec3(0.0f);
	};
}