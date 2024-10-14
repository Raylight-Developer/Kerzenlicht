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

KL::Confirm<dvec1> KL::OBJECT::DATA::Mesh::f_rayPicking(const dmat4& transform_matrix, const dvec3& ray_origin, const dvec3& ray_direction) {
	dvec1 closest_hit = MAX_DIST;
	for (const KL::OBJECT::DATA::MESH::Face* face : faces) {
		if (face->vertices.size() == 3) {
			const dvec3 vert_a = MESH::Vertex::transformed(transform_matrix, face->vertices[0]->position);
			const dvec3 vert_b = MESH::Vertex::transformed(transform_matrix, face->vertices[1]->position);
			const dvec3 vert_c = MESH::Vertex::transformed(transform_matrix, face->vertices[2]->position);

			auto hit = MESH::Face::f_rayTriangleIntersection(ray_origin, ray_direction, vert_a, vert_b, vert_c);
			if (hit) {
				if (hit.data < closest_hit) {
					closest_hit = hit.data;
				}
			}

		}
	}
	if (closest_hit == MAX_DIST) {
		return Confirm<dvec1>();
	}
	return Confirm(closest_hit);
}

bool KL::OBJECT::DATA::Mesh::f_rayAnyHit(const dmat4& transform_matrix, const dvec3& ray_origin, const dvec3& ray_direction) {
	for (const KL::OBJECT::DATA::MESH::Face* face : faces) {
		if (face->vertices.size() == 3) {
			const dvec3 vert_a = MESH::Vertex::transformed(transform_matrix, face->vertices[0]->position);
			const dvec3 vert_b = MESH::Vertex::transformed(transform_matrix, face->vertices[1]->position);
			const dvec3 vert_c = MESH::Vertex::transformed(transform_matrix, face->vertices[2]->position);

			auto hit = MESH::Face::f_rayTriangleIntersection(ray_origin, ray_direction, vert_a, vert_b, vert_c);
			if (hit) {
				return true;
			}
		}
	}
	return false;
}

KL::Confirm<dvec1>  KL::OBJECT::DATA::MESH::Face::f_rayTriangleIntersection(const dvec3& ray_origin, const dvec3& ray_direction, const dvec3& vert_a, const dvec3& vert_b, const dvec3& vert_c) {
	const dvec3 v1v0 = vert_b - vert_a;
	const dvec3 v2v0 = vert_c - vert_a;
	const dvec3 rov0 = ray_origin - vert_a;

	const dvec3 n = glm::cross(v1v0, v2v0);
	const dvec3 q = glm::cross(rov0, ray_direction);
	const dvec1 d = 1.0 / glm::dot(ray_direction, n);
	const dvec1 u = d * glm::dot(-q, v2v0);
	const dvec1 v = d * glm::dot(q, v1v0);
	const dvec1 t_dist = d * glm::dot(-n, rov0);

	if (u < 0.0 || v < 0.0 || (u + v) > 1.0) {
		return Confirm<dvec1>();
	}

	return Confirm(t_dist);
}

dvec3 KL::OBJECT::DATA::MESH::Vertex::transformed(const dmat4& matrix, const dvec3& position) {
	const dvec4 vert4 = matrix * dvec4(position, 1.0);
	return dvec3(vert4.x, vert4.y, vert4.z) / vert4.w;
}