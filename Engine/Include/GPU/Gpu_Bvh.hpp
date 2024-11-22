#pragma once

#include "Shared.hpp"

#include "Gpu_Triangle.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::GPU {
	struct alignas(16) Bvh {
		vec3 p_min;
		uint uptr;
		vec3 p_max;
		uint tri_count;
		// 32

		Bvh(
			const vec3& p_min = vec3(MAX_VEC1),
			const vec3& p_max = vec3(MIN_VEC1),
			const uint& uptr = 0U,
			const uint& tri_count = 0U
		);

		void growToInclude(const vec3& min, const vec3& max);
		vec3 getSize() const;
		vec3 getCenter() const;
	};

	namespace BVH {
		struct Tri {
			vec3   p_min;
			vec3   p_max;
			vec3   center;
			uint64 index;
			GPU::Triangle tri;

			Tri(
				const vec3& p_min,
				const vec3& p_max,
				const vec3& center,
				const uint64& index,
				const GPU::Triangle& tri
			);
		};

		struct Builder {
			vector<GPU::Triangle> triangles;
			vector<BVH::Tri> bvh_tris;
			vector<GPU::Bvh> node_list;
			GPU::Bvh mesh_bounds;

			Builder(const vector<GPU::Triangle>& triangles, const uint& depth);

			void splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth);
			void splitAxis(const GPU::Bvh& node, const uint& start, const uint& count, uint8& axis, vec1& pos, vec1& cost) const;
			float splitEval(const uint8& splitAxis, const vec1& splitPos, const uint& start, const uint& count) const;
			static float nodeCost(const vec3& size, const uint& numTriangles);
		};
	}
}