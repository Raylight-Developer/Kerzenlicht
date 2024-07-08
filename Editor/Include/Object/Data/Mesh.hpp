#pragma once

#include "Include.hpp"

namespace CLASS::OBJECT::DATA {
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
		map<uint8, unordered_map<MESH::Face*, vector<dvec2>>> uvs;
		map<uint8, unordered_map<MESH::Face*, vector<dvec3>>> normals;

		Mesh();
		~Mesh();
	};
}