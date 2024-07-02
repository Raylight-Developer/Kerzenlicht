#include "Camera.hpp"

GPU_Camera::GPU_Camera() {
	width  = 1920;
	height = 1080;

	focal_length = 0.05f;
	focal_angle  = 40.0f;
	sensor_size  = 0.036f;

	position = vec3( -3.0f ,2.0f, -3.0f );
	rotation = vec3( 15.0f, 45.0f, 0.0f);

	x_vector = vec3( 1.0f, 0.0f, 0.0f);
	y_vector = vec3( 0.0f, 1.0f, 0.0f);
	z_vector = vec3( 0.0f, 0.0f, 1.0f);

	f_compileVectors();
}

void GPU_Camera::f_move(const double& i_x, const double& i_y, const double& i_z, const double& i_speed) {
	position += i_x * i_speed * x_vector;
	position += i_y * i_speed * y_vector;
	position += i_z * i_speed * z_vector;
}

dmat4 GPU_Camera::f_getViewMatrix() {
	return lookAt(position, position + z_vector, y_vector);
}

void GPU_Camera::f_rotate(const double& i_yaw, const double& i_pitch) {
	rotation += dvec3(i_yaw, i_pitch, 0);

	if (rotation.y > 89.0) rotation.y = 89.0;
	if (rotation.y < -89.0) rotation.y = -89.0;

	f_compileVectors();
}

void GPU_Camera::f_compileVectors() {
	const mat4 matrix = glm::yawPitchRoll(rotation.y * DEG_RAD, rotation.x * DEG_RAD, rotation.z * DEG_RAD);

	x_vector = matrix[0];
	y_vector = matrix[1];
	z_vector = matrix[2];
}

void GPU_Camera::f_compile() {
	projection_center = position + focal_length * z_vector;
	projection_u = normalize(cross(z_vector, y_vector)) * sensor_size;
	projection_v = normalize(cross(projection_u, z_vector)) * sensor_size;
}