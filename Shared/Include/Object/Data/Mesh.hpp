#pragma once

#include "Include.hpp"

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
		vector<MESH::Vertex*> vertices;
		vector<MESH::Face*> faces;
		map<MESH::Face*, vector<dvec2>> uvs;
		map<MESH::Face*, vector<dvec3>> normals;
		map<MESH::Face*, KL::Shader*> materials;
		map<string, vector<MESH::Vertex*>> vertex_groups;

		Mesh();
		~Mesh();
	};
}