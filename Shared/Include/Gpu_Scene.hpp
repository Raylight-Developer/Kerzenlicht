#pragma once

#include "Shared.hpp"

#include "File.hpp"

namespace KL {
	struct GPU_Scene;
	struct alignas(16) GPU_Triangle;
	struct alignas(16) GPU_Texture;
	struct alignas(16) GPU_BVH;

	GPU_Triangle faceToGpuTri(const mat4& matrix, KL::OBJECT::DATA::Mesh* mesh, KL::OBJECT::DATA::MESH::Face* face);

	struct GPU_Scene {
		vector<GPU_Triangle>      triangles;
		vector<GPU_BVH>           bvh_nodes;
		vector<GPU_Texture>       textures;
		vector<uint>              texture_data;

		//Camera                    camera;
		//vector<GPU_Material>      materials;
		//vector<GPU_Light>         lights;
		//vector<GPU_Spline_Point>  spline_controls;
		//vector<GPU_Spline>        splines;
		//vector<GPU_Curve>         curves;
		//vector<GPU_Vertex>        vertices;
		//vector<GPU_Mesh>          meshes;

		GPU_Scene();

		void f_init();

		void updateTextures();
		void f_tickUpdate();

		uint64 bvhNodesSize() const;
		uint64 texturesSize() const;
		uint64 trianglesSize() const;
		uint64 textureDataSize() const;

		void print() const;
		void printInfo() const;
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
		vec3 padding = vec3(0.0f);
		uint material;
		// 112

		GPU_Triangle(
			const vec3& pos_a = vec3(0.0f),
			const vec3& pos_b = vec3(0.0f),
			const vec3& pos_c = vec3(0.0f),
			const vec3& normal_a = vec3(0.0f),
			const vec3& normal_b = vec3(0.0f),
			const vec3& normal_c = vec3(0.0f),
			const vec2& uv_a = vec2(0.0f),
			const vec2& uv_b = vec2(0.0f),
			const vec2& uv_c = vec2(0.0f),
			const uint& material = 0
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
			uv_c_y(uv_c.y),
			material(material)
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
		uint uptr;   // 16
		vec3 p_max;
		uint tri_count; // 32

		GPU_BVH(
			const vec3& p_min     = vec3(0.0f),
			const vec3& p_max     = vec3(0.0f),
			const uint& uptr   = 0U,
			const uint& tri_count = 0U
		) :
			p_min(p_min),
			p_max(p_max),
			uptr(uptr),
			tri_count(tri_count)
		{}

		void growToInclude(const vec3& min, const vec3& max);
		vec3 getSize();
		vec3 getCenter();
		Lace print() const;
	};

	enum struct Texture_Fromat {
		RGBA8,
		RGBA16,
		RGBA32
	};

	struct alignas(16) GPU_Texture {
		uint start;
		uint width;
		uint height;
		uint format; // 32

		GPU_Texture(
			const uint& start  = 0U,
			const uint& width  = 0U,
			const uint& height = 0U,
			const uint& format = 0U
		) :
			start(start),
			width(width),
			height(height),
			format(format)
		{}
		Lace print() const;
	};

	struct BVH_Builder {
		vector<GPU_Triangle> triangles;
		vector<BVH_Triangle> bvh_tris;
		vector<GPU_BVH> node_list;
		GPU_BVH mesh_bounds;

		BVH_Builder(const vector<GPU_Triangle>& triangles, const uint& depth);

		void splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth);
		void splitAxis(const GPU_BVH& node, const uint& start, const uint& count, uint8& axis, vec1& pos, vec1& cost) const;
		float splitEval(const uint8& splitAxis, const vec1& splitPos, const uint& start, const uint& count) const;
		static float nodeCost(const vec3& size, const uint& numTriangles);
	};
}