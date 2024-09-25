#pragma once

#include "Shared.hpp"

#include "Object/Data/Mesh.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::GPU {
	struct alignas(16) Triangle {
		vec3 pos_a;
		vec1 uv_a_x;
		vec3 normal_a;
		vec1 uv_a_y;
		// 32
		vec3 pos_b;
		vec1 uv_b_x;
		vec3 normal_b;
		vec1 uv_b_y;
		// 64
		vec3 pos_c;
		vec1 uv_c_x;
		vec3 normal_c;
		vec1 uv_c_y;
		// 96
		vec3 padding = vec3(0.0f);
		uint material;
		// 112

		Triangle(
			const vec3& pos_a = vec3(0.0f),
			const vec3& pos_b = vec3(0.0f),
			const vec3& pos_c = vec3(0.0f),
			const vec3& normal_a = vec3(0.0f),
			const vec3& normal_b = vec3(0.0f),
			const vec3& normal_c = vec3(0.0f),
			const vec2& uv_a = vec2(0.0f),
			const vec2& uv_b = vec2(0.0f),
			const vec2& uv_c = vec2(0.0f),
			const uint& material = 0
		);
	};

	Triangle faceToGpuTri(const mat4& matrix, KL::OBJECT::DATA::Mesh* mesh, KL::OBJECT::DATA::MESH::Face* face);
}