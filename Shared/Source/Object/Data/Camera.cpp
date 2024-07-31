#include "Object/Data/Camera.hpp"

#include "Object/Object.hpp"

CLASS::OBJECT::DATA::Camera::Camera() {
	focal_angle  = 50.0;
	focal_length = 0.05;
	sensor_size  = 0.036;
}

void CLASS::OBJECT::DATA::Camera::f_compile(CLASS::Scene* scene, CLASS::Object* object) {
	const dmat4 matrix = glm::yawPitchRoll(object->transform.euler_rotation.y * DEG_RAD, object->transform.euler_rotation.x * DEG_RAD, object->transform.euler_rotation.z * DEG_RAD);
	const dvec3 x_vector = matrix[0];
	const dvec3 y_vector = matrix[1];
	const dvec3 z_vector = -matrix[2];
	
	projection_center = object->transform.position + focal_length * z_vector;
	projection_u = normalize(cross(z_vector, y_vector)) * sensor_size ;
	projection_v = normalize(cross(projection_u, z_vector)) * sensor_size;
}