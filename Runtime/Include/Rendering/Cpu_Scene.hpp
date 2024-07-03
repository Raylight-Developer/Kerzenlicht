#pragma once

#include "Include.hpp"
#include "Lace.hpp"
#include "Ops.hpp"

#include "Camera.hpp"

struct CPU_Scene;

struct Material;

struct Empty;

struct BVH;

struct Light;

struct Curve_Point;
struct Curve;

struct Vertex;
struct Triangle;
struct Mesh;

// Basic Shapes
struct Box;
struct AABB;
struct Cone;
struct Torus;
struct Sphere;
struct Capsule;
struct Cylinder;
struct Ellipsoid;
struct Uneven_Capsule;

CPU_Scene f_parseCPUData();
void f_parseCamera   (const vector<vector<string>>& data, Camera&                camera);
void f_parseMaterial (const vector<vector<string>>& data, map<string, Material>& materials);
void f_parseEmpty    (const vector<vector<string>>& data, map<string, Empty>&    empties);
void f_parseLight    (const vector<vector<string>>& data, map<string, Light>&    lights);
void f_parseCurve    (const vector<vector<string>>& data, map<string, Curve>&    curves);
void f_parseMesh     (const vector<vector<string>>& data, map<string, Mesh>&     meshes);
vec3 f_averageVertexPos(const vector<Vertex>& vertices);

struct CPU_Scene {
	uint32 render_steps;
	uint32 render_step_stop;
	Camera                camera;
	map<string, Material> materials;
	map<string, Empty>    empties;
	map<string, Light>    lights;
	map<string, Curve>    curves;
	map<string, Mesh>     meshes;

	CPU_Scene(
		const uint32& render_steps = 6,
		const uint32& render_step_stop = 0,
		const Camera&                camera    = Camera(),
		const map<string, Material>& materials = {},
		const map<string, Empty>&    empties   = {},
		const map<string, Light>&    lights    = {},
		const map<string, Curve>&    curves    = {},
		const map<string, Mesh>&     meshes    = {}
	) :
		render_steps     (render_steps),
		render_step_stop (render_step_stop),
		camera    (camera),
		materials (materials),
		empties   (empties),
		lights    (lights),
		curves    (curves),
		meshes    (meshes)
	{}

	void print() const;
};

struct Material {
	vec3   color;

	Material(
		const vec3& color = vec3(1.0f)
	) :
		color(color)
	{}
};

struct BVH {
	vec3 p_min;
	vec3 p_max;
	BVH* node_a;
	BVH* node_b;

	bool is_leaf;
	vector<uint32> pointers; // if is_leaf and pointers is empty, pointers is entire stack

	BVH(
		const vec3&             p_min    = vec3(-1.0f),
		const vec3&             p_max    = vec3(1.0f),
		BVH*                    node_a   = nullptr,
		BVH*                    node_b   = nullptr,
		const bool&             is_leaf  = false,
		const vector<uint32>&   pointers = {}
	) :
		p_min   (p_min),
		p_max   (p_max),
		node_a  (node_a),
		node_b  (node_b),
		is_leaf (is_leaf),
		pointers(pointers)
	{}
	~BVH() {
		if (node_a != nullptr) delete node_a;
		if (node_b != nullptr) delete node_b;
	}
};

struct Empty {
	vec3 position;
	vec3 rotation;
	vec3 scale;

	Empty(
		const vec3& position,
		const vec3& rotation,
		const vec3& scale
	) :
		position(position),
		rotation(rotation),
		scale(scale)
	{}
};

struct Light {
	vec3  color;
	float power;

	Light(
		const vec3&  color = vec3(1.0f),
		const float& power = 1.0f
	) :
		color(color),
		power(power)
	{}
};

struct Curve_Point {
	vec3 pos;
	vec1 radius;

	Curve_Point(
		const vec3& pos,
		const vec1& radius
	) :
		pos(pos),
		radius(radius)
	{}
};
struct Curve {
	vector<vector<Curve_Point>> splines;

	Curve(
		const vector<vector<Curve_Point>>& splines = {}
	) :
		splines(splines)
	{}
};

struct Vertex {
	vec3 position;

	Vertex(
		const vec3& position = vec3(0.0f)
	) :
		position(position)
	{}
};
struct Triangle {
	uvec3 vertex_pointer;
	vec3  normal_a;
	vec3  normal_b;
	vec3  normal_c;
	vec2  uv_a;
	vec2  uv_b;
	vec2  uv_c;

	Triangle(
		const uvec3&  vertex_pointer = uvec3(0U),
		const vec3& normal_a = vec3(0.0f),
		const vec3& normal_b = vec3(0.0f),
		const vec3& normal_c = vec3(0.0f),
		const vec2& uv_a = vec2(0.0f),
		const vec2& uv_b = vec2(0.0f),
		const vec2& uv_c = vec2(0.0f)
	) :
		vertex_pointer  (vertex_pointer),
		normal_a(normal_a),
		normal_b(normal_b),
		normal_c(normal_c),
		uv_a(uv_a),
		uv_b(uv_b),
		uv_c(uv_c)
	{}
};
struct Mesh {
	BVH bvh;
	vector<Vertex>   vertices;
	vector<Triangle> triangles;

	Mesh(
		const BVH& bvh                    = BVH(),
		const vector<Vertex>& vertices    = {},
		const vector<Triangle>& triangles = {}
	) :
		bvh(bvh),
		vertices(vertices),
		triangles(triangles)
	{}
};

struct Box {
	vec3 pos;
	vec3 rot;
	vec3 size;

	Box(
		const vec3& pos,
		const vec3& rot,
		const vec3& size
	) :
		pos(pos),
		rot(rot),
		size(size)
	{}
};
struct AABB {
	vec3 min;
	vec3 max;

	AABB(
		const vec3& min = vec3(0.0f),
		const vec3& max = vec3(0.0f)
	) :
		min(min),
		max(max)
	{}
};
struct Cone {
	vec3 pos_a;
	vec3 pos_b;
	vec1 radius_a;
	vec1 radius_b;

	Cone(
		const vec3& pos_a,
		const vec3& pos_b,
		const vec1& radius_a,
		const vec1& radius_b
	) :
		pos_a(pos_a),
		pos_b(pos_b),
		radius_a(radius_a),
		radius_b(radius_b)
	{}
};
struct Torus {
	vec3 pos;
	vec3 rot;
	vec1 inner_radius;
	vec1 outer_radius;

	Torus(
		const vec3& pos,
		const vec3& rot,
		const vec1& inner_radius,
		const vec1& outer_radius
	) :
		pos(pos),
		rot(rot),
		inner_radius(inner_radius),
		outer_radius(outer_radius)
	{}
};
struct Sphere {
	vec3 pos;
	vec1 radius;

	Sphere(
		const vec3& pos,
		const vec1& radius
	) :
		pos(pos),
		radius(radius)
	{}
};
struct Capsule {
	vec3 pos_a;
	vec3 pos_b;
	vec1 radius;

	Capsule(
		const vec3& pos_a,
		const vec3& pos_b,
		const vec1& radius
	) :
		pos_a(pos_a),
		pos_b(pos_b),
		radius(radius)
	{}
};
struct Cylinder {
	vec3 pos_a;
	vec3 pos_b;
	vec1 radius;

	Cylinder(
		const vec3& pos_a,
		const vec3& pos_b,
		const vec1& radius
	) :
		pos_a(pos_a),
		pos_b(pos_b),
		radius(radius)
	{}
};
struct Ellipsoid {
	vec3 pos;
	vec3 rot;
	vec3 radii;

	Ellipsoid(
		const vec3& pos,
		const vec3& rot,
		const vec3& radii
	) :
		pos(pos),
		rot(rot),
		radii(radii)
	{}
};
struct Uneven_Capsule {
	vec3 pos_a;
	vec3 pos_b;
	vec1 radius_a;
	vec1 radius_b;

	Uneven_Capsule(
		const vec3& pos_a,
		const vec3& pos_b,
		const vec1& radius_a,
		const vec1& radius_b
	) :
		pos_a(pos_a),
		pos_b(pos_b),
		radius_a(radius_a),
		radius_b(radius_b)
	{}
};