#include "Object/Data/Mesh.hpp"

KL::OBJECT::DATA::Mesh::Mesh() {
	vertices = vector<MESH::Vertex*>();
	faces = vector<MESH::Face*>();
}

KL::OBJECT::DATA::Mesh::~Mesh() {
	for (MESH::Vertex* vertex : vertices) delete vertex;
	for (MESH::Face* face : faces) delete face;
}

vector<vec1> KL::OBJECT::DATA::Mesh::faceToArray(MESH::Face* face, const Mesh* mesh, const mat4& matrix) {
	vector<vec1> result;

	// Transform vertices
	const vec4 vert4_a = matrix * vec4(d_to_f(face->vertices[0]->position), 1.0);
	const vec4 vert4_b = matrix * vec4(d_to_f(face->vertices[1]->position), 1.0);
	const vec4 vert4_c = matrix * vec4(d_to_f(face->vertices[2]->position), 1.0);
	const vec3 vert_a = vec3(vert4_a) / vert4_a.w;
	const vec3 vert_b = vec3(vert4_b) / vert4_b.w;
	const vec3 vert_c = vec3(vert4_c) / vert4_c.w;

	// Initialize normals and UVs
	vec3 nor_a = vec3(0.0f);
	vec3 nor_b = vec3(0.0f);
	vec3 nor_c = vec3(0.0f);
	vec2 uv_a = vec2(0.0f);
	vec2 uv_b = vec2(0.0f);
	vec2 uv_c = vec2(0.0f);

	// Get UVs if available
	if (mesh->uvs.size() > 0) {
		uv_a = d_to_f(mesh->uvs.at(face)[0]);
		uv_b = d_to_f(mesh->uvs.at(face)[1]);
		uv_c = d_to_f(mesh->uvs.at(face)[2]);
	}

	// Get normals if available
	if (mesh->normals.size() > 0) {
		const vec4 nor4_a = matrix * vec4(d_to_f(mesh->normals.at(face)[0]), 1.0);
		const vec4 nor4_b = matrix * vec4(d_to_f(mesh->normals.at(face)[1]), 1.0);
		const vec4 nor4_c = matrix * vec4(d_to_f(mesh->normals.at(face)[2]), 1.0);
		nor_a = vec3(nor4_a) / nor4_a.w;
		nor_b = vec3(nor4_b) / nor4_b.w;
		nor_c = vec3(nor4_c) / nor4_c.w;
	}

	// Pack the data into the vector in the desired format
	auto packVertexData = [&](const vec3& vert, const vec3& nor, const vec2& uv) {
		result.push_back(vert.x);
		result.push_back(vert.y);
		result.push_back(vert.z);
		result.push_back(nor.x);
		result.push_back(nor.y);
		result.push_back(nor.z);
		result.push_back(uv.x);
		result.push_back(uv.y);
		};

	packVertexData(vert_a, nor_a, uv_a);
	packVertexData(vert_b, nor_b, uv_b);
	packVertexData(vert_c, nor_c, uv_c);

	return result;
}