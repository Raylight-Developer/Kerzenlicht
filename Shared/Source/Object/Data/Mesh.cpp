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

bool KL::OBJECT::DATA::MESH::Face::f_rayTriangleIntersection(const mat4& transform_matrix, const vec3& ray_origin, const vec3& ray_direction, const vec3& vert_a, const vec3& vert_b, const vec3& vert_c, vec1& ray_length) {
	const vec4 vert4_a = transform_matrix * vec4(vert_a, 1);
	const vec4 vert4_b = transform_matrix * vec4(vert_b, 1);
	const vec4 vert4_c = transform_matrix * vec4(vert_c, 1);
	vec3 trans_vert_a = vec3(vert4_a.x, vert4_a.y, vert4_a.z);
	vec3 trans_vert_b = vec3(vert4_b.x, vert4_b.y, vert4_b.z);
	vec3 trans_vert_c = vec3(vert4_c.x, vert4_c.y, vert4_c.z);
	if (vert4_a.w != 1.0f && vert4_a.w != 0.0f) {
		trans_vert_a /= vert4_a.w;
	}
	if (vert4_b.w != 1.0f && vert4_b.w != 0.0f) {
		trans_vert_b /= vert4_b.w;
	}
	if (vert4_c.w != 1.0f && vert4_c.w != 0.0f) {
		trans_vert_c /= vert4_c.w;
	}

	const vec3 v1v0 = trans_vert_b - trans_vert_a;
	const vec3 v2v0 = trans_vert_c - trans_vert_a;
	const vec3 rov0 = ray_origin - trans_vert_a;

	const vec3 n = glm::cross(v1v0, v2v0);
	const vec3 q = glm::cross(rov0, ray_direction);
	const vec1 d = 1.0f / glm::dot(ray_direction, n);
	const vec1 u = d * glm::dot(-q, v2v0);
	const vec1 v = d * glm::dot(q, v1v0);
	const vec1 t = d * glm::dot(-n, rov0);

	if (u < 0.0 || v < 0.0 || (u + v) > 1.0) {
		return false;
	}

	ray_length = t;
	return true;
}