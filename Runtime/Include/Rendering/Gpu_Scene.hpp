#pragma once

#include "Include.hpp"

#include "Lace.hpp"
#include "Ops.hpp"
#include "Session.hpp"

#include "Camera.hpp"

struct GPU_Scene;
struct alignas(16) GPU_Triangle;
struct alignas(16) GPU_BVH;

enum struct Parse_Type {
	NONE,
	BUILD_STEPS,
	NODE_TREE,
	MATERIAL,
	HEADER,
	OBJECT,
	SCENE,
	DATA
};

struct GPU_Scene {
	vector<GPU_Triangle>      triangles;
	vector<GPU_BVH>           bvh_nodes;

	//Camera                    camera;
	//vector<GPU_Material>      materials;
	//vector<GPU_Light>         lights;
	//vector<GPU_Spline_Point>  spline_controls;
	//vector<GPU_Spline>        splines;
	//vector<GPU_Curve>         curves;
	//vector<GPU_Vertex>        vertices;
	//vector<GPU_Mesh>          meshes;

	GPU_Scene(const string& data_file_path, const string& shader_file_path = "");

	void f_loadHeader     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadNodeTree   (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadMaterial   (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadData       (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadAtmosphere (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadPrimitive  (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadSkeleton   (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadCamera     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadVolume     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadCurve      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadEmpty      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadForce      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadGroup      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadLight      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadMesh       (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadSfx        (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadVfx        (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadObject     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadScene      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
	void f_loadBuild      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);

	void print() const;
	void printInfo(const uint64& max_size) const;
};

struct alignas(16) GPU_Triangle {
	vec3 pos_a;
	vec3 normal_a;
	vec2 uv_a;
	// 32
	vec3 pos_b;
	vec3 normal_b;
	vec2 uv_b;
	// 64
	vec3 pos_c;
	vec3 normal_c;
	vec2 uv_c;

	GPU_Triangle(
		const vec3& pos_a = vec3(0.0f),
		const vec3& pos_b = vec3(0.0f),
		const vec3& pos_x = vec3(0.0f),
		const vec3& normal_a = vec3(0.0f),
		const vec3& normal_b = vec3(0.0f),
		const vec3& normal_c = vec3(0.0f),
		const vec2& uv_a = vec2(0.0f),
		const vec2& uv_b = vec2(0.0f),
		const vec2& uv_c = vec2(0.0f)
	) :
		pos_a(pos_a),
		pos_b(pos_b),
		pos_c(pos_c),
		normal_a(normal_a),
		normal_b(normal_b),
		normal_c(normal_c),
		uv_a(uv_a),
		uv_b(uv_b),
		uv_c(uv_c)
	{}

	Lace print() const;
};

struct alignas(16) GPU_BVH {
	vec3 p_min;
	uint pointer;   // 16
	vec3 p_max;
	uint tri_count; // 32

	GPU_BVH(const vec3& p_min, const vec3& p_max, const uint& pointer, const uint& tri_count) :
		p_min(p_min), p_max(p_max), pointer(pointer), tri_count(tri_count)
	{}

	Lace print() const;
};