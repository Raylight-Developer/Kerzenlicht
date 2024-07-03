#pragma once

#include "Include.hpp"

#include "Camera.hpp"
#include "Cpu_Scene.hpp"

struct GPU_Scene;

struct alignas(16) GPU_Material;

struct alignas(16) GPU_Light;

struct alignas(16) GPU_Spline_Point;
struct alignas(16) GPU_Spline;
struct alignas(16) GPU_Curve;

struct alignas(16) GPU_Vertex;
struct alignas(16) GPU_Triangle;
struct alignas(16) GPU_Mesh;

struct alignas(16) GPU_BVH;

GPU_Scene f_parseGPUData(const CPU_Scene& cpu_data);

struct GPU_Scene {
	Camera                camera;
	vector<GPU_Material>      materials;
	vector<GPU_Light>         lights;
	vector<GPU_Spline_Point>  spline_controls;
	vector<GPU_Spline>        splines;
	vector<GPU_Curve>         curves;
	vector<GPU_Vertex>        vertices;
	vector<GPU_Triangle>      triangles;
	vector<GPU_Mesh>          meshes;
	vector<GPU_BVH>           bvh_nodes;

	GPU_Scene(
		const Camera&                camera          = Camera(),
		const vector<GPU_Material>&      materials       = {},
		const vector<GPU_Light>&         lights          = {},
		const vector<GPU_Spline_Point>&  spline_controls = {},
		const vector<GPU_Spline>&        splines         = {},
		const vector<GPU_Curve>&         curves          = {},
		const vector<GPU_Vertex>&        vertices        = {},
		const vector<GPU_Triangle>&      triangles       = {},
		const vector<GPU_Mesh>&          meshes          = {},
		const vector<GPU_BVH>&           bvh_nodes       = {}
	) :
		camera         (camera),
		materials      (materials),
		lights         (lights),
		spline_controls(spline_controls),
		splines        (splines),
		curves         (curves),
		vertices       (vertices),
		triangles      (triangles),
		meshes         (meshes),
		bvh_nodes      (bvh_nodes)
	{}

	void print() const;
	void printInfo(const uint64& max_size) const;
};

struct alignas(16) GPU_Material {
	vec3 albedo;
	vec1 specular;
	// 16
	vec3 emission;
	vec1 anisotropy;
	// 32
	vec1 metallic;
	vec1 roughness;
	vec1 subsurface;
	vec1 specularTint;
	// 48
	vec1 sheen;
	vec3 sheenTint;
	// 64
	vec1 clearcoat;
	vec1 clearcoatRoughness;
	vec1 transmission;
	vec1 ior;
	// 80
	vec3 attenuationColor;
	vec1 attenuationDistance;
	// 96
	vec1 ax;
	vec1 ay;
	vec1 alpha;
	uint unlit;
	// 112
	vec3 f0;
	uint thinwalled;

	GPU_Material(
		const vec3& albedo = vec3(1.0f)
	) :
		albedo(albedo)
	{
		specular = 1.0f;
		emission = vec3(0.0f);
		anisotropy = 0.0f;
		metallic = 0.0f;
		roughness = 0.25f;
		subsurface = 0.25f;
		specularTint = 0.0f;
		sheen = 0.0f;
		sheenTint = vec3(0.0f);
		clearcoat = 0.0f;
		clearcoatRoughness = 0.0f;
		transmission = 0.0f;
		ior = 1.3f;
		attenuationColor = vec3(0.0f);
		attenuationDistance = 0.0f;
		ax = 0.5f;
		ay = 0.5f;
		alpha = 1.0f;
		unlit = 0U;
		f0 = vec3(0.0f);
		thinwalled = 0U;
	}
};

struct alignas(16) GPU_Light {
	vec3  color;
	float power;

	GPU_Light(
		const vec3&  color = vec3(1.0f),
		const float& power = 1.0f
	) :
		color(color),
		power(power)
	{}
};

struct alignas(16) GPU_Spline_Point {
	vec3 position;
	vec1 radius;

	GPU_Spline_Point(
		const vec3& position = vec3( 0.0f),
		const vec1& radius   = 1.0f
	) :
		position(position),
		radius  (radius)
	{}
};
struct alignas(16) GPU_Spline {
	uvec2 handle_pointer;
	uint padding_a = 0U;
	uint padding_b = 0U;

	GPU_Spline(
		const uvec2& handle_pointer = uvec2(0U)
	) :
		handle_pointer(handle_pointer)
	{}
};
struct alignas(16) GPU_Curve {
	uvec2 spline_pointer;
	uint padding_a = 0U;
	uint padding_b = 0U;

	GPU_Curve(
		const uvec2& spline_pointer = uvec2(0U)
	) :
		spline_pointer(spline_pointer)
	{}
};

struct alignas(16) GPU_Vertex {
	vec3 position;
	uint padding = 0U;

	GPU_Vertex(
		const vec3& position = vec3(0.0)
	) :
		position(position)
	{}
};
struct alignas(16) GPU_Triangle {
	uvec3 vertex_pointer;
	uint  material_pointer;
	vec4  normal_a;
	vec4  normal_b;
	vec4  normal_c;
	vec2  uv_a;
	vec2  uv_b;
	vec2  uv_c;
	uint padding_a = 0U;
	uint padding_b = 0U;

	GPU_Triangle(
		const uvec3&  vertex_pointer   = uvec3(0U),
		const uint& material_pointer = 0U,
		const vec3& normal_a = vec3(0.0f),
		const vec3& normal_b = vec3(0.0f),
		const vec3& normal_c = vec3(0.0f),
		const vec2& uv_a = vec2(0.0f),
		const vec2& uv_b = vec2(0.0f),
		const vec2& uv_c = vec2(0.0f)
	) :
		vertex_pointer  (vertex_pointer),
		material_pointer(material_pointer),
		normal_a(vec4(normal_a, 0.0f)),
		normal_b(vec4(normal_b, 0.0f)),
		normal_c(vec4(normal_c, 0.0f)),
		uv_a(uv_a),
		uv_b(uv_b),
		uv_c(uv_c)
	{}
};
struct alignas(16) GPU_Mesh {
	uvec2 triangle_pointer;
	uint padding_a = 0U;
	uint padding_b = 0U;

	GPU_Mesh(
		const uvec2& triangle_pointer = uvec2(0U)
	) :
		triangle_pointer(triangle_pointer)
	{}
};

struct alignas(16) GPU_BVH {
	vec3 p_min;
	uint child_a;
	vec3 p_max;
	uint child_b;

	uint mesh_pointer;
	uint leaf;
	uint padding_a = 0U;
	uint padding_b = 0U;

	GPU_BVH(const vec3& p_min, const vec3& p_max, const uint& pointer_a, const uint& pointer_b, const uint& mesh_pointer, const bool& leaf) :
		p_min(p_min), p_max(p_max), child_a(pointer_a), child_b(pointer_b), mesh_pointer(mesh_pointer), leaf(leaf)
	{}
};