#pragma once

#include "Include.hpp"
#include "Ops.hpp"

#include "Shader/Shader.hpp"

#include "Object/Data.hpp"

namespace KL::OBJECT::DATA {
	namespace MESH {
		struct Vertex {
			dvec3 position;

			Vertex(const dvec3& position)
				: position(position)
			{}

			static dvec3 transformed(const dmat4& matrix, const dvec3& position);
		};
		struct Face {
			vector<Vertex*> vertices;
			static KL::Confirm<dvec1> f_rayTriangleIntersection(const dvec3& ray_origin, const dvec3& ray_direction, const dvec3& vert_a, const dvec3& vert_b, const dvec3& vert_c);
		};
	}
	struct Mesh : Data {
		bool cpu_update;

		vector<MESH::Vertex*> vertices;
		vector<MESH::Face*> faces;
		vector<KL::Shader*> shader_slots;

		map<MESH::Face*, vector<dvec2>> uvs;
		map<MESH::Face*, vector<dvec3>> normals;
		map<MESH::Face*, uint> shaders;
		map<string, vector<MESH::Vertex*>> vertex_groups;

		Mesh();
		~Mesh();

		static vector<vec1> faceToArray(MESH::Face* face, const Mesh* mesh, const mat4& matrix, const mat3& rotation_matrix);
		static void packVertexData(vector<vec1>& array, const vec3& vert, const vec3& nor, const vec2& uv);

		KL::Confirm<dvec1> f_rayPicking(const dmat4& transform_matrix, const dvec3& ray_origin, const dvec3& ray_direction);
		bool f_rayAnyHit(const dmat4& transform_matrix, const dvec3& ray_origin, const dvec3& ray_direction);
	};
}