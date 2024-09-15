#include "Object/Data/Mesh.hpp"

KL::OBJECT::DATA::Mesh::Mesh() {
	cpu_update = true;
}

KL::OBJECT::DATA::Mesh::~Mesh() {
	for (MESH::Vertex* vertex : vertices) delete vertex;
	for (MESH::Face* face : faces) delete face;
}

vector<vec1> KL::OBJECT::DATA::Mesh::faceToArray(MESH::Face* face, const Mesh* mesh, const mat4& matrix, const mat3& rotation_matrix) {
	vector<vec1> result;

	// Transform vertices
	const vec3 vert_a = d_to_f(face->vertices[0]->position);
	const vec3 vert_b = d_to_f(face->vertices[1]->position);
	const vec3 vert_c = d_to_f(face->vertices[2]->position);

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

	// Get Normals if available
	if (mesh->normals.size() > 0) {
		nor_a = d_to_f(mesh->normals.at(face)[0]);
		nor_b = d_to_f(mesh->normals.at(face)[1]);
		nor_c = d_to_f(mesh->normals.at(face)[2]);
	}
	else {
		const vec3 edge1 = vert_b - vert_a;
		const vec3 edge2 = vert_c - vert_a;
		const vec3 normal = glm::normalize(glm::cross(edge1, edge2));
		nor_a = normal;
		nor_b = normal;
		nor_c = normal;
	}

	Mesh::packVertexData(result, vert_a, nor_a, uv_a);
	Mesh::packVertexData(result, vert_b, nor_b, uv_b);
	Mesh::packVertexData(result, vert_c, nor_c, uv_c);

	return result;
}

void KL::OBJECT::DATA::Mesh::packVertexData(vector<vec1>& array, const vec3& vert, const vec3& nor, const vec2& uv) {
	array.push_back(vert.x);
	array.push_back(vert.y);
	array.push_back(vert.z);
	array.push_back(nor.x);
	array.push_back(nor.y);
	array.push_back(nor.z);
	array.push_back(uv.x);
	array.push_back(uv.y);
}