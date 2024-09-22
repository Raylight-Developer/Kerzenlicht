#pragma once

#include "Shared.hpp"

namespace KL::GPU {
	struct alignas(16) Bvh {
		vec3 p_min;
		uint uptr;
		vec3 p_max;
		uint tri_count;
		// 32

		Bvh(
			const vec3& p_min = vec3(0.0f),
			const vec3& p_max = vec3(0.0f),
			const uint& uptr = 0U,
			const uint& tri_count = 0U
		);

		void growToInclude(const vec3& min, const vec3& max);
		vec3 getSize();
		vec3 getCenter();
	};
}