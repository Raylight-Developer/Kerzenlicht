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
			const uint& tri_count = MAX_UINT32
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

			Tri(const vec3& p_min, const vec3& p_max, const vec3& center, const uint64& index, const GPU::Triangle& tri);
		};

		struct BLAS {
			vector<GPU::Triangle> mesh_triangles;
			vector<BVH::Tri> bvh_tris;
			vector<GPU::Bvh> mesh_blas;
			GPU::Bvh mesh_bounds;

			BLAS(const vector<GPU::Triangle>& triangles, const uint& depth);

			void splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth);
			void splitAxis(const GPU::Bvh& node, const uint& start, const uint& count, uint8& axis, vec1& pos, vec1& cost) const;
			float splitEval(const uint8& splitAxis, const vec1& splitPos, const uint& start, const uint& count) const;
			static float nodeCost(const vec3& size, const uint& numTriangles);
		};

		struct TLAS {
			vector<GPU::Bvh> mesh_blas;
			vector<GPU::Bvh> mesh_tlas;

			TLAS(const vector<GPU::Bvh>& mesh_blas, const uint& depth);

			void splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth);
			void splitAxis(const GPU::Bvh& node, const uint& start, const uint& count, uint8& axis, vec1& pos, vec1& cost) const;
			float splitEval(const uint8& splitAxis, const vec1& splitPos, const uint& start, const uint& count) const;
			static float nodeCost(const vec3& size, const uint& numTriangles);
		};
	}
}