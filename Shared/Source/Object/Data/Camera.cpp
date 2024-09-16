#include "Object/Data/Camera.hpp"

#include "Object/Object.hpp"

KL::OBJECT::DATA::Camera::Camera() {
	type = Type::CAMERA;

	view_angle  = 70.0;

	projection_uv = dvec3(0.0);
	projection_u  = dvec3(0.0);
	projection_v  = dvec3(0.0);
}

void KL::OBJECT::DATA::Camera::f_updateRayVectors(KL::Scene* scene, KL::Object* object) {
	dmat4 rotation_matrix;
	switch (object->transform.rotation_type) {
		case KL::Rotation_Type::QUATERNION: {
			rotation_matrix = glm::mat4_cast(object->transform.quat_rotation);
			break;
		}
		case KL::Rotation_Type::XYZ: {
			rotation_matrix = glm::yawPitchRoll(object->transform.euler_rotation.y * DEG_RAD, object->transform.euler_rotation.x * DEG_RAD, object->transform.euler_rotation.z * DEG_RAD);
			break;
		}
	}
	const dvec3 x_vector =  rotation_matrix[0];
	const dvec3 y_vector =  rotation_matrix[1];
	const dvec3 z_vector = -rotation_matrix[2];

	const dvec1 projectionPlaneHalfWidth = tan((view_angle * DEG_RAD) / 2.0);

	projection_uv = object->transform.position + z_vector;
	projection_u = normalize(cross(z_vector, y_vector)) * projectionPlaneHalfWidth * 2.0;
	projection_v = normalize(cross(projection_u, z_vector)) * projectionPlaneHalfWidth * 2.0;
}

dmat4 KL::OBJECT::DATA::Camera::glViewMatrix(const KL::Object* object) const {
	dmat4 rotation_matrix;
	switch (object->transform.rotation_type) {
		case KL::Rotation_Type::QUATERNION: {
			rotation_matrix = glm::mat4_cast(object->transform.quat_rotation);
			break;
		}
		case KL::Rotation_Type::XYZ: {
			rotation_matrix = glm::yawPitchRoll(object->transform.euler_rotation.y * DEG_RAD, object->transform.euler_rotation.x * DEG_RAD, object->transform.euler_rotation.z * DEG_RAD);
			break;
		}
	}
	const dvec3 x_vector =  rotation_matrix[0];
	const dvec3 y_vector =  rotation_matrix[1];
	const dvec3 z_vector = -rotation_matrix[2];

	const dvec3 camera_position = object->transform.position;
	const dmat4 view_matrix = glm::lookAt(camera_position, camera_position + z_vector, y_vector);

	return view_matrix;
}

dmat4 KL::OBJECT::DATA::Camera::glProjectionMatrix(const dvec1& aspect_ratio) const {
	const dvec1 fov = 2.0 * atan(tan((view_angle * DEG_RAD) / 2.0) / aspect_ratio);
	return glm::perspective(fov, aspect_ratio, 0.01, 1000.0);
}