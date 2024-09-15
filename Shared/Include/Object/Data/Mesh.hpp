#pragma once

#include "Include.hpp"
#include "Ops.hpp"

#include "Shader/Shader.hpp"

namespace KL::OBJECT::DATA {
	namespace MESH {
		struct Vertex {
			dvec3 position;

			Vertex(const dvec3& position)
				: position(position)
			{}
		};
		struct Face {
			vector<Vertex*> vertices;
		};
	}
	struct Mesh {
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
	};
}