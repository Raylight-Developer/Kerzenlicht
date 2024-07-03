#pragma once

#include "Include.hpp"

struct Camera {
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

	Camera();

	void f_move(const double& x, const double& y, const double& z, const double& speed);
	void f_rotate(const double& yaw, const double& pitch);
	void f_compileVectors();
};