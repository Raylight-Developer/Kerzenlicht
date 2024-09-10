#include "Object/Data/Camera.hpp"

#include "Object/Object.hpp"

KL::OBJECT::DATA::Camera::Camera() {
	focal_angle  = 50.0;
	focal_length = 0.05;
	sensor_size  = 0.036;

	projection_uv = dvec3(0.0);
	projection_u  = dvec3(0.0);
	projection_v  = dvec3(0.0);
}

void KL::OBJECT::DATA::Camera::compile(KL::Scene* scene, KL::Object* object) {
	const dmat4 matrix = glm::yawPitchRoll(object->transform.euler_rotation.y * DEG_RAD, object->transform.euler_rotation.x * DEG_RAD, object->transform.euler_rotation.z * DEG_RAD);
	const dvec3 x_vector = matrix[0];
	const dvec3 y_vector = matrix[1];
	const dvec3 z_vector = -matrix[2];
	
	projection_uv = object->transform.position + focal_length * z_vector;
	projection_u  = normalize(cross(z_vector, y_vector)) * sensor_size ;
	projection_v  = normalize(cross(projection_u, z_vector)) * sensor_size;
}

void KL::OBJECT::DATA::Camera::updateFocalAngle() {
	const dvec1 argument = sensor_size / (2.0 * focal_length);
	const dvec1 fovRadians = 2.0 * std::atan(argument);
	focal_angle = fovRadians * RAD_DEG;
}

mat4 KL::OBJECT::DATA::Camera::glViewMatrix(const KL::Object* object, const dvec1& aspect_ratio) const {
	const mat4 rotation_matrix = glm::yawPitchRoll(object->transform.euler_rotation.y * DEG_RAD, object->transform.euler_rotation.x * DEG_RAD, object->transform.euler_rotation.z * DEG_RAD);
	
	const vec3 x_vector = rotation_matrix[0];
	const vec3 y_vector = rotation_matrix[1];
	const vec3 z_vector = -rotation_matrix[2];

	const vec3 camera_position = object->transform.position;
	const mat4 view_matrix = glm::lookAt(camera_position, camera_position + z_vector, y_vector);

	const mat4 projection_matrix = glm::perspective(d_to_f(focal_angle * DEG_RAD), d_to_f(aspect_ratio), 0.01f, 1000.0f);
	return projection_matrix * view_matrix;
}