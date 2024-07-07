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
	vec1 uv_a_x;
	vec3 normal_a;
	vec1 uv_a_y;
	// 32
	vec3 pos_b;
	vec1 uv_b_x;
	vec3 normal_b;
	vec1 uv_b_y;
	// 64
	vec3 pos_c;
	vec1 uv_c_x;
	vec3 normal_c;
	vec1 uv_c_y;
	// 96

	GPU_Triangle(
		const vec3& pos_a = vec3(0.0f),
		const vec3& pos_b = vec3(0.0f),
		const vec3& pos_c = vec3(0.0f),
		const vec3& normal_a = vec3(0.0f),
		const vec3& normal_b = vec3(0.0f),
		const vec3& normal_c = vec3(0.0f),
		const vec2& uv_a = vec2(0.0f),
		const vec2& uv_b = vec2(0.0f),
		const vec2& uv_c = vec2(0.0f)
	) :
		pos_a(pos_a),
		uv_a_x(uv_a.x),
		normal_a(normal_a),
		uv_a_y(uv_a.y),
		pos_b(pos_b),
		uv_b_x(uv_b.x),
		normal_b(normal_b),
		uv_b_y(uv_b.y),
		pos_c(pos_c),
		uv_c_x(uv_c.x),
		normal_c(normal_c),
		uv_c_y(uv_c.y)
	{}

	Lace print() const;
};

struct BVH_Triangle {
	vec3   p_min;
	vec3   p_max;
	vec3   center;
	uint64 index;
	GPU_Triangle tri;

	BVH_Triangle(
		const vec3& p_min       = vec3(-0.5f),
		const vec3& p_max       = vec3( 0.5f),
		const vec3& center      = vec3( 0.0f),
		const uint64& index     = 0ULL,
		const GPU_Triangle& tri = GPU_Triangle()
	) :
		center(center),
		p_min(p_min),
		p_max(p_max),
		index(index),
		tri(tri)
	{}
};

struct alignas(16) GPU_BVH {
	vec3 p_min;
	uint pointer;   // 16
	vec3 p_max;
	uint tri_count; // 32

	GPU_BVH(
		const vec3& p_min     = vec3(0.0f),
		const vec3& p_max     = vec3(0.0f),
		const uint& pointer   = 0U,
		const uint& tri_count = 0U
	) :
		p_min(p_min),
		p_max(p_max),
		pointer(pointer),
		tri_count(tri_count)
	{}

	void f_growToInclude(const vec3& min, const vec3& max);
	vec3 CalculateBoundsSize() { return p_max - p_min; };
	vec3 CalculateBoundscenter() { return (p_min + p_max) / 2.0f; };
	Lace print() const;
};

struct BVH_Builder {
	vector<GPU_Triangle> triangles;
	vector<BVH_Triangle> bvh_tris;
	vector<GPU_BVH> node_list;
	GPU_BVH mesh_bounds;

	BVH_Builder(const vector<GPU_Triangle>& triangles);

	void f_splitBvh(const uint64& parentIndex, const uint64& triGlobalStart, const uint64& triNum, const uint64& depth = 0) {
		const int MaxDepth = 8;
		GPU_BVH parent = node_list[parentIndex];
		vec3 size = parent.CalculateBoundsSize();
		float parentCost = NodeCost(size, triNum);

		uint16 splitAxis;
		float splitPos;
		float cost;
		
		ChooseSplit(parent, triGlobalStart, triNum, splitAxis, splitPos, cost);

		if (cost < parentCost && depth < MaxDepth) {
			GPU_BVH boundsLeft;
			GPU_BVH boundsRight;
			uint64 numOnLeft = 0;

			for (int i = triGlobalStart; i < triGlobalStart + triNum; i++) {
				BVH_Triangle tri = bvh_tris[i];
				if (tri.center[splitAxis] < splitPos) {
					boundsLeft.f_growToInclude(tri.p_min, tri.p_max);

					BVH_Triangle swap = bvh_tris[triGlobalStart + numOnLeft];
					bvh_tris[triGlobalStart + numOnLeft] = tri;
					bvh_tris[i] = swap;
					numOnLeft++;
				}
				else {
					boundsRight.f_growToInclude(tri.p_min, tri.p_max);
				}
			}

			uint64 numOnRight = triNum - numOnLeft;
			uint64 triStartLeft = triGlobalStart;
			uint64 triStartRight = triGlobalStart + numOnLeft;

			// Split parent into two children
			node_list.push_back(GPU_BVH(boundsLeft.p_min, boundsLeft.p_max, triStartLeft));
			uint64 childIndexRight = node_list.size();
			uint64 childIndexLeft = childIndexRight - 1;
			node_list.push_back(GPU_BVH(boundsRight.p_min, boundsRight.p_max, triStartRight));

			// Update parent
			parent.pointer = childIndexLeft;
			node_list[parentIndex] = parent;

			// Recursively split children
			f_splitBvh(childIndexLeft, triGlobalStart, numOnLeft, depth + 1);
			f_splitBvh(childIndexRight, triGlobalStart + numOnLeft, numOnRight, depth + 1);
		}
		else {
			// Parent is actually leaf, assign all triangles to it
			parent.pointer = triGlobalStart;
			parent.tri_count = triNum;
			node_list[parentIndex] = parent;
		}
	}

	void ChooseSplit(const GPU_BVH& node, const int& start, const int& count, uint16& axis, float& pos, float& cost) const {
		if (count <= 1) {
			axis = 0;
			pos = 0;
			cost = MAX_VEC1;
			return;
		}

		float bestSplitPos = 0;
		int bestSplitAxis = 0;
		const int numSplitTests = 5;

		float bestCost = MAX_VEC1;

		// Estimate best split pos
		for (uint axis = 0; axis < 3; axis++)
		{
			for (uint i = 0; i < numSplitTests; i++)
			{
				float splitT = (i + 1) / (numSplitTests + 1.0f);
				float splitPos = glm::lerp(node.p_min[axis], node.p_max[axis], splitT);
				float cost = EvaluateSplit(axis, splitPos, start, count);
				if (cost < bestCost)
				{
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

	float EvaluateSplit(const int64& splitAxis, const float& splitPos, const int64& start, const int64& count) const {
		GPU_BVH boundsLeft;
		GPU_BVH boundsRight;
		int numOnLeft = 0;
		int numOnRight = 0;

		for (int i = start; i < start + count; i++)
		{
			BVH_Triangle tri = bvh_tris[i];
			if (tri.center[splitAxis] < splitPos)
			{
				boundsLeft.f_growToInclude(tri.p_min, tri.p_max);
				numOnLeft++;
			}
			else
			{
				boundsRight.f_growToInclude(tri.p_min, tri.p_max);
				numOnRight++;
			}
		}

		float costA = NodeCost(boundsLeft.CalculateBoundsSize(), numOnLeft);
		float costB = NodeCost(boundsRight.CalculateBoundsSize(), numOnRight);
		return costA + costB;
	}

	static float NodeCost(const vec3& size, const uint64& numTriangles) {
		float halfArea = size.x * size.y + size.x * size.z + size.y * size.z;
		return halfArea * numTriangles;
	}
};