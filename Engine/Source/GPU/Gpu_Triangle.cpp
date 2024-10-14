#include "GPU/Gpu_Triangle.hpp"

#include "Utils/Session.hpp"

KL::GPU::Triangle::Triangle(const vec3& pos_a, const vec3& pos_b, const vec3& pos_c, const vec3& normal_a, const vec3& normal_b, const vec3& normal_c, const vec2& uv_a, const vec2& uv_b, const vec2& uv_c, const uint& material) :
	pos_a(pos_a),
	uv_a_x(uv_a.x),
	normal_a(normal_a),
	uv_a_y(uv_a.y),
	pos_b(pos_b),
	uv_b_x(uv_b.x),
	normal_b(normal_b),
	uv_b_y(uv_b.y),
	pos_c(pos_c),
	uv_c_x(uv_c.x),
	normal_c(normal_c),
	uv_c_y(uv_c.y),
	material(material)
{}

KL::GPU::Triangle KL::GPU::faceToGpuTri(const mat4& matrix, KL::OBJECT::DATA::Mesh* mesh, KL::OBJECT::DATA::MESH::Face* face) {
	const vec4 vert4_a = matrix * vec4(d_to_f(face->vertices[0]->position), 1.0);
	const vec4 vert4_b = matrix * vec4(d_to_f(face->vertices[1]->position), 1.0);
	const vec4 vert4_c = matrix * vec4(d_to_f(face->vertices[2]->position), 1.0);
	const vec3 vert_a = vec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
	const vec3 vert_b = vec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
	const vec3 vert_c = vec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
	vec3 nor_a = vec3(0.0f);
	vec3 nor_b = vec3(0.0f);
	vec3 nor_c = vec3(0.0f);
	vec2 uv_a = vec2(0.0f);
	vec2 uv_b = vec2(0.0f);
	vec2 uv_c = vec2(0.0f);

	uint material_index = MAX_UINT32;
	auto confirm = f_getMapValue(mesh->shaders, face);
	if (confirm) {
		if (mesh->shader_slots.size() > mesh->shaders[face]) {
			auto sub_confirm = f_getVectorIndex(FILE->shaders, mesh->shader_slots[confirm.data]);
			if (sub_confirm) {
				material_index = ul_to_u(sub_confirm.data);
			}
		}
	}

	if (mesh->uvs.size() > 0) {
		uv_a = d_to_f(mesh->uvs[face][0]);
		uv_b = d_to_f(mesh->uvs[face][1]);
		uv_c = d_to_f(mesh->uvs[face][2]);
	}

	if (mesh->normals.size() > 0) {
		const vec4 nor4_a = matrix * vec4(d_to_f(mesh->normals[face][0]), 1.0);
		const vec4 nor4_b = matrix * vec4(d_to_f(mesh->normals[face][1]), 1.0);
		const vec4 nor4_c = matrix * vec4(d_to_f(mesh->normals[face][2]), 1.0);
		nor_a = vec3(nor4_a.x, nor4_a.y, nor4_a.z) / nor4_a.w;
		nor_b = vec3(nor4_b.x, nor4_b.y, nor4_b.z) / nor4_b.w;
		nor_c = vec3(nor4_c.x, nor4_c.y, nor4_c.z) / nor4_c.w;
	}

	return KL::GPU::Triangle(
		vert_a,
		vert_b,
		vert_c,
		nor_a,
		nor_b,
		nor_c,
		uv_a,
		uv_b,
		uv_c,
		material_index
	);
}