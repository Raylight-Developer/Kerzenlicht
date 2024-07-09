#include "Camera.hpp"

Camera::Camera() {
	width  = 1920;
	height = 1080;

	focal_length = 0.05;
	focal_angle  = 50.0;
	sensor_size  = 0.036;

	transform = CLASS::Transform(dvec3( -2.5, 1.5, -2.5 ), dvec3( 20, 45, 0 ));

	x_vector = dvec3( 1.0f, 0.0f, 0.0f);
	y_vector = dvec3( 0.0f, 1.0f, 0.0f);
	z_vector = dvec3( 0.0f, 0.0f, 1.0f);

	compileVectors();
}

void Camera::move(const double& x, const double& y, const double& z, const double& speed) {
	transform.position += x * speed * x_vector;
	transform.position += y * speed * y_vector;
	transform.position += z * speed * z_vector;
}

void Camera::rotate(const double& yaw, const double& pitch) {
	transform.euler_rotation += dvec3(pitch, yaw, 0);

	if (transform.euler_rotation.x > 89.0) transform.euler_rotation.x = 89.0;
	if (transform.euler_rotation.x < -89.0) transform.euler_rotation.x = -89.0;

	compileVectors();
}

void Camera::compileVectors() {
	const dmat4 matrix = glm::yawPitchRoll(transform.euler_rotation.y * DEG_RAD, transform.euler_rotation.x * DEG_RAD, transform.euler_rotation.z * DEG_RAD);
	x_vector = matrix[0];
	y_vector = matrix[1];
	z_vector = matrix[2];
}

void Camera::compile() {
	projection_center = transform.position + focal_length * z_vector;
	projection_u = normalize(cross(z_vector, y_vector)) * sensor_size ;
	projection_v = normalize(cross(projection_u, z_vector)) * sensor_size;
}