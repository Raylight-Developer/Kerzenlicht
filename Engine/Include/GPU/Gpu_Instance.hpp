#pragma once

#include "Shared.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::GPU {
	struct alignas(16) Object_Instance {
		uint object_index;
		vec3 padding = vec3(0.0f);

		Object_Instance(const mat4& object_matrix, const uint& object_index);
	};
}