#pragma once

#include "Include.hpp"

#include "Core/File.hpp"
#include "Ops.hpp"

#include "Object/Data/Camera.hpp"

namespace GUI {
	namespace WORKSPACE {
		class Viewport_CPU_Renderer;
	}
}

struct Ray {
	dvec3 origin;
	dvec3 direction;

	Ray(
		const dvec3& origin = dvec3(0.0),
		const dvec3& direction = dvec3(0.0, 0.0, -1.0)
	) :
		origin(origin),
		direction(direction)
	{};
};

struct Hit {
	bool hit;
	bool inside;

	dvec3 pos;
	dvec1 t_dist;

	dvec3 uvw;
	dvec3 normal_a;
	dvec3 normal_b;
	dvec3 normal_c;
	dvec2 uv_a;
	dvec2 uv_b;
	dvec2 uv_c;

	Hit(
		bool hit              = false,
		bool inside           = false,
		const dvec3& pos      = dvec3(0.0),
		const dvec1& t_dist   = MAX_DIST,
		const dvec3& uvw      = dvec3(0.2, 0.2, 0.6),
		const dvec3& normal_a = dvec3(1.0, 0.0, 0.0),
		const dvec3& normal_b = dvec3(0.0, 1.0, 0.0),
		const dvec3& normal_c = dvec3(0.0, 0.0, 1.0),
		const dvec2& uv_a     = dvec2(0.5),
		const dvec2& uv_b     = dvec2(0.5),
		const dvec2& uv_c     = dvec2(0.5)
	) :
		hit(hit),
		inside(inside),
		pos(pos),
		t_dist(t_dist),
		uvw(uvw),
		normal_a(normal_a),
		normal_b(normal_b),
		normal_c(normal_c),
		uv_a(uv_a),
		uv_b(uv_b),
		uv_c(uv_c)
	{}
};

struct Render_Kernel {
	const dvec1 aspect_ratio;
	const dvec1 inv_aspect_ratio;
	const uvec2 resolution;

	Render_Kernel(GUI::WORKSPACE::Viewport_CPU_Renderer* viewport);

	// Init
	dvec3 f_renderPixel(const dvec1& x, const dvec1& y);
	dvec3 f_renderSample(const dvec2& uv);

	// Shading
	dvec3 f_radiance(const Hit& hit_data);

	Hit f_sceneIntersection(const Ray& ray);

	// Ray
	Ray f_cameraRay(const dvec2& uv);

	// Intersections
	bool f_rayTriangleIntersection(const Ray& ray, const dvec3& vertex_a, const dvec3& vertex_b, const dvec3& vertex_c, dvec1& t_dist);


	dvec3 f_barycentricCoords(const dvec3& hit_pos, const dvec3& vertex_a, const dvec3& vertex_b, const dvec3& vertex_c);
};