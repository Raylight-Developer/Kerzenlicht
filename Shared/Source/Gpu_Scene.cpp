﻿#include "Gpu_Scene.hpp"

#ifdef COMPILE_EDITOR
	#include "Core/Editor_File.hpp"
#elif COMPILE_RENDERER
	#include "Core/Render_File.hpp"
#else
	#include "File.hpp"
#endif

#include "Session.hpp"

KL::GPU_Scene::GPU_Scene() {
	triangles = {};
	bvh_nodes = {};
	textures = {};
	texture_data = {};
}

void KL::GPU_Scene::print() const {
	Lace data;
	data << "Triangles " << triangles.size() NL;
	for (const GPU_Triangle& tri : triangles)
		data TAB << tri.print() NL;
	data << "BVHs " << bvh_nodes.size() NL;
	for (const GPU_BVH& bvh : bvh_nodes)
		data TAB << bvh.print() NL;
	data << "Textures " << textures.size() NL;
	for (const GPU_Texture& texture : textures)
		data TAB << texture.print() NL;
	data << "Texture Data " << texture_data.size() NL TAB;
	for (const uint& pixel : texture_data)
		data SP << pixel;
	data NL;
	cout << data.str() << endl;
}

void KL::GPU_Scene::printInfo() const {
	cout << "GPU Data:" << endl;
	printSize("  Triangles    ", triangles);
	printSize("  BVHs         ", bvh_nodes);
	printSize("  Textures     ", textures);
	printSize("  Texture Data ", texture_data);
}

void KL::GPU_Scene::updateTextures() { // TODO handle different formats
	for (SHADER::Texture* texture : FILE->textures) {
		vector<uint> data = texture->toRgba8Texture();
		textures.push_back(GPU_Texture(ul_to_u(texture_data.size()), texture->resolution.x, texture->resolution.y, 0));
		texture_data.insert(texture_data.end(), data.begin(), data.end());
	}
}

void KL::GPU_Scene::f_tickUpdate() {
	triangles.clear();
	bvh_nodes.clear();

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		object->f_compileMatrix();
		vector<GPU_Triangle> mesh_triangles;
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			KL::OBJECT::DATA::Mesh* mesh = object->data->getMesh();
			for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
				mesh_triangles.push_back(faceToGpuTri(object->transform_matrix, mesh, face));
			}
		}
		else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
			const KL::OBJECT::DATA::Group* group = object->data->getGroup();
			for (KL::Object* sub_object : group->objects) {
				sub_object->f_compileMatrix();
				sub_object->transform_matrix *= object->transform_matrix;
				if (sub_object->data->type == KL::OBJECT::DATA::Type::MESH) {
					KL::OBJECT::DATA::Mesh* mesh = sub_object->data->getMesh();
					for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
						mesh_triangles.push_back(faceToGpuTri(sub_object->transform_matrix, mesh, face));
					}
				}
			}
		}
		const uint64 triangle_offset = triangles.size();
		const uint bvh_depth = d_to_u(glm::log2(ul_to_d(mesh_triangles.size()) / 64.0));

		BVH_Builder bvh_build = BVH_Builder(mesh_triangles, bvh_depth);
		bvh_nodes.insert(bvh_nodes.end(), bvh_build.node_list.begin(), bvh_build.node_list.end());
		triangles.insert(triangles.end(), bvh_build.triangles.begin(), bvh_build.triangles.end());
		//LOG ENDL << "Triangle Count: " << mesh_triangles.size() << "  BVH Depth: " << bvh_depth << "  BVH Nodes: " << bvh_nodes.size();
	}
}

uint64 KL::GPU_Scene::bvhNodesSize() const {
	return sizeof(GPU_BVH) * bvh_nodes.size();
}

uint64 KL::GPU_Scene::texturesSize() const {
	return sizeof(GPU_Texture) * textures.size();
}

uint64 KL::GPU_Scene::trianglesSize() const {
	return sizeof(GPU_Triangle) * triangles.size();
}

uint64 KL::GPU_Scene::textureDataSize() const {
	return sizeof(uint) * texture_data.size();
}

KL::Lace KL::GPU_Triangle::print() const {
	Lace lace;
	lace << "Tri: (" << pos_a<< ") : (" << normal_a << ") : (" << vec2(uv_a_x, uv_a_y) << ") | (" << pos_b << ") : (" << normal_b << ") : (" << vec2(uv_b_x, uv_b_y) << ") | (" << pos_c << ") : (" << normal_c << ") : (" << vec2(uv_c_x, uv_c_y) << ")";
	return lace;
}

void KL::GPU_BVH::growToInclude(const vec3& min, const vec3& max) {
	p_min.x = min.x < p_min.x ? min.x : p_min.x;
	p_min.y = min.y < p_min.y ? min.y : p_min.y;
	p_min.z = min.z < p_min.z ? min.z : p_min.z;
	p_max.x = max.x > p_max.x ? max.x : p_max.x;
	p_max.y = max.y > p_max.y ? max.y : p_max.y;
	p_max.z = max.z > p_max.z ? max.z : p_max.z;
}

vec3 KL::GPU_BVH::getSize() {
	return p_max - p_min;
}

vec3 KL::GPU_BVH::getCenter() {
	return (p_min + p_max) / 2.0f;
}

KL::Lace KL::GPU_BVH::print() const {
	Lace lace;
	lace << "Bvh: (" << p_min<< ") | (" << p_max << ") | " << uptr << " | " << tri_count;
	return lace;
}

KL::BVH_Builder::BVH_Builder(const vector<GPU_Triangle>& triangles, const uint& depth) :
	triangles(triangles)
{
	for (uint i = 0; i < len32(triangles); i++) {
		const vec3 a = triangles[i].pos_a;
		const vec3 b = triangles[i].pos_b;
		const vec3 c = triangles[i].pos_c;
		const vec3 center = (a + b + c) / 3.0f;
		const vec3 max = glm::max(glm::max(a, b), c);
		const vec3 min = glm::min(glm::min(a, b), c);
		bvh_tris.push_back(BVH_Triangle(min, max, center, i, triangles[i]));
		mesh_bounds.growToInclude(min, max);
	}

	node_list.push_back(mesh_bounds);
	splitBvh(0, 0, len32(triangles), depth);

	this->triangles.clear();
	for (uint i = 0; i < len32(bvh_tris); i++) {
		this->triangles.push_back(bvh_tris[i].tri);
	}
}

void KL::BVH_Builder::splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth) {
	GPU_BVH parent = node_list[parentIndex];
	vec3 size = parent.getSize();
	vec1 parentCost = nodeCost(size, triNum);

	uint8 split_axis;
	vec1 splitPos;
	vec1 cost;

	splitAxis(parent, triGlobalStart, triNum, split_axis, splitPos, cost);

	if (cost < parentCost && depth > 0) {
		GPU_BVH boundsLeft;
		GPU_BVH boundsRight;
		uint numOnLeft = 0;

		for (uint i = triGlobalStart; i < triGlobalStart + triNum; i++) {
			BVH_Triangle tri = bvh_tris[i];
			if (tri.center[split_axis] < splitPos) {
				boundsLeft.growToInclude(tri.p_min, tri.p_max);

				BVH_Triangle swap = bvh_tris[triGlobalStart + numOnLeft];
				bvh_tris[triGlobalStart + numOnLeft] = tri;
				bvh_tris[i] = swap;
				numOnLeft++;
			}
			else {
				boundsRight.growToInclude(tri.p_min, tri.p_max);
			}
		}

		uint numOnRight = triNum - numOnLeft;
		uint triStartLeft = triGlobalStart;
		uint triStartRight = triGlobalStart + numOnLeft;

		node_list.push_back(GPU_BVH(boundsLeft.p_min, boundsLeft.p_max, triStartLeft));
		uint childIndexRight = len32(node_list);
		uint childIndexLeft = childIndexRight - 1;
		node_list.push_back(GPU_BVH(boundsRight.p_min, boundsRight.p_max, triStartRight));

		parent.uptr = childIndexLeft;
		node_list[parentIndex] = parent;

		splitBvh(childIndexLeft, triGlobalStart, numOnLeft, depth - 1);
		splitBvh(childIndexRight, triGlobalStart + numOnLeft, numOnRight, depth - 1);
	}
	else {
		parent.uptr = triGlobalStart;
		parent.tri_count = triNum;
		node_list[parentIndex] = parent;
	}
}

void KL::BVH_Builder::splitAxis(const GPU_BVH& node, const uint& start, const uint& count, uint8& axis, vec1& pos, vec1& cost) const {
	if (count <= 1) {
		axis = 0;
		pos = 0.0f;
		cost = MAX_VEC1;
		return;
	}

	vec1 bestSplitPos = 0;
	uint8 bestSplitAxis = 0;
	const uint8 numSplitTests = 5;

	vec1 bestCost = MAX_VEC1;

	for (uint8 axis = 0; axis < 3; axis++) {
		for (uint8 i = 0; i < numSplitTests; i++) {
			vec1 splitT = (u_to_f(i) + 1.0f) / (u_to_f(numSplitTests) + 1.0f);
			vec1 splitPos = glm::lerp(node.p_min[axis], node.p_max[axis], splitT);
			vec1 cost = splitEval(axis, splitPos, start, count);
			if (cost < bestCost) {
				bestCost = cost;
				bestSplitPos = splitPos;
				bestSplitAxis = axis;
			}
		}
	}

	axis = bestSplitAxis;
	pos = bestSplitPos;
	cost = bestCost;
}

vec1 KL::BVH_Builder::splitEval(const uint8& splitAxis, const vec1& splitPos, const uint& start, const uint& count) const {
	GPU_BVH boundsLeft;
	GPU_BVH boundsRight;
	uint numOnLeft = 0;
	uint numOnRight = 0;

	for (uint i = start; i < start + count; i++) {
		BVH_Triangle tri = bvh_tris[i];
		if (tri.center[splitAxis] < splitPos) {
			boundsLeft.growToInclude(tri.p_min, tri.p_max);
			numOnLeft++;
		}
		else {
			boundsRight.growToInclude(tri.p_min, tri.p_max);
			numOnRight++;
		}
	}

	vec1 costA = nodeCost(boundsLeft.getSize(), numOnLeft);
	vec1 costB = nodeCost(boundsRight.getSize(), numOnRight);
	return costA + costB;
}

vec1 KL::BVH_Builder::nodeCost(const vec3& size, const uint& numTriangles) {
	const vec1 halfArea = size.x * size.y + size.x * size.z + size.y * size.z;
	return halfArea * numTriangles;
}

KL::Lace KL::GPU_Texture::print() const {
	Lace lace;
	lace << "Texture: (" << start<< ") | (" << width << " x " << height << ") | " << format;
	return lace;
}

KL::GPU_Triangle KL::faceToGpuTri(const mat4& matrix, KL::OBJECT::DATA::Mesh* mesh, KL::OBJECT::DATA::MESH::Face* face) {
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
	auto [exists_a, value] = f_getMapValue(mesh->shaders, face, 0U);
	if (exists_a) {
		if (mesh->shader_slots.size() > mesh->shaders[face]) {
			auto [exists, index] = f_getVectorIndex(FILE->shaders, mesh->shader_slots[value]);
			if (exists) {
				material_index = ul_to_u(index);
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
	return KL::GPU_Triangle(
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