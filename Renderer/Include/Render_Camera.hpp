#pragma once

#include "Include.hpp"
#include "Ops.hpp"

struct Render_Camera {
	uint16 width;
	uint16 height;

	dvec1 focal_length;
	dvec1 focal_angle;
	dvec1 sensor_size;

	CLASS::Transform transform;

	dvec3 x_vector;
	dvec3 y_vector;
	dvec3 z_vector;

	dvec3 projection_center;
	dvec3 projection_u;
	dvec3 projection_v;

	Render_Camera();

	void move(const double& x, const double& y, const double& z, const double& speed);
	void rotate(const double& yaw, const double& pitch);
	void compileVectors();
	void compile();
};