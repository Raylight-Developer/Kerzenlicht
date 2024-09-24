#include "Object/Data/Camera.hpp"

#include "Object/Object.hpp"

KL::OBJECT::DATA::Camera::Camera() {
	type = Type::CAMERA;

	focal_distance = 2.0;
	view_angle = 70.0;
	aperture = 2.7;
}

void KL::OBJECT::DATA::Camera::f_updateRayVectors(KL::Scene* scene, KL::Object* object, mat3& vectors, mat3& projections) {
	object->transform.f_computeVectors();

	vectors[0] = d_to_f(object->transform.x_vec);
	vectors[1] = d_to_f(object->transform.y_vec);
	vectors[2] = d_to_f(object->transform.z_vec);

	const vec1 projectionPlaneHalfWidth = d_to_f(tan((glm::radians(view_angle)) / 2.0));

	projections[0] = d_to_f(object->transform.position) + vectors[2];
	projections[1] = normalize(cross(vectors[2], vectors[1])) * projectionPlaneHalfWidth * 2.0f;
	projections[2] = normalize(cross(projections[1], vectors[2])) * projectionPlaneHalfWidth * 2.0f;
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