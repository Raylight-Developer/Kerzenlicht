#pragma once

#include "Include.hpp"

struct GPU_Camera {
	uint16 width;
	uint16 height;

	dvec1 focal_length;
	dvec1 focal_angle;
	dvec1 sensor_size;

	dvec3 position;
	dvec3 rotation;
	dvec3 x_vector;
	dvec3 y_vector;
	dvec3 z_vector;

	dvec3 projection_center;
	dvec3 projection_u;
	dvec3 projection_v;

	GPU_Camera();

	void f_move(const double& i_x, const double& i_y, const double& i_z, const double& i_speed);
	void f_rotate(const double& i_yaw, const double& i_pitch);
	dmat4 f_getViewMatrix();
	void f_compileVectors();
	void f_compile();
};