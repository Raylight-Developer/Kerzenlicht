#include "Camera.hpp"

Camera::Camera() {
	width  = 1920;
	height = 1080;

	focal_length = 0.05;
	focal_angle  = 40.0;
	sensor_size  = 0.036;

	position = dvec3( 0, 0, -3.5 );
	rotation = dvec3( 90, 0,  0 );

	x_vector = dvec3( 1.0f, 0.0f, 0.0f);
	y_vector = dvec3( 0.0f, 1.0f, 0.0f);
	z_vector = dvec3( 0.0f, 0.0f, 1.0f);

	compileVectors();
}

void Camera::move(const double& x, const double& y, const double& z, const double& speed) {
	position += x * speed * x_vector;
	position += y * speed * y_vector;
	position += z * speed * z_vector;
}

void Camera::rotate(const double& yaw, const double& pitch) {
	rotation += dvec3(yaw, pitch, 0);

	if (rotation.y > 89.0) rotation.y = 89.0;
	if (rotation.y < -89.0) rotation.y = -89.0;

	compileVectors();
}

void Camera::compileVectors() {
	z_vector = normalize(dvec3(
		cos(rotation.x * DEG_RAD) * cos(rotation.y * DEG_RAD),
		sin(rotation.y * DEG_RAD),
		sin(rotation.x * DEG_RAD) * cos(rotation.y * DEG_RAD)
	));
	x_vector = normalize(cross(z_vector, dvec3(0, 1, 0)));
	y_vector = normalize(cross(x_vector, z_vector));
}