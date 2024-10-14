#include "Object/Data/Camera.hpp"

#include "Object/Object.hpp"

KL::OBJECT::DATA::Camera::Camera() {
	type = Type::CAMERA;

	focal_distance = 2.0;
	view_angle = 70.0;
	aperture = 2.7;

	f_setupLenses(CAMERA::Lens_Setup::NONE);
}

void KL::OBJECT::DATA::Camera::glProperties(KL::Scene* scene, KL::Object* object, mat3& vectors, mat3& projections) const {
	object->transform.f_computeVectors();

	vectors[0] =  d_to_f(object->transform.x_vec);
	vectors[1] =  d_to_f(object->transform.y_vec);
	vectors[2] = -d_to_f(object->transform.z_vec);

	const vec1 projectionPlaneHalfWidth = d_to_f(tan((glm::radians(view_angle)) / 2.0));

	projections[0] = d_to_f(object->transform.position) + vectors[2];
	projections[1] = normalize(cross(vectors[2], vectors[1])) * projectionPlaneHalfWidth * 2.0f;
	projections[2] = normalize(cross(projections[1], vectors[2])) * projectionPlaneHalfWidth * 2.0f;
}

void KL::OBJECT::DATA::Camera::glProperties(KL::Scene* scene, KL::Object* object, dmat3& vectors, dmat3& projections) const {
	object->transform.f_computeVectors();

	vectors[0] =  object->transform.x_vec;
	vectors[1] =  object->transform.y_vec;
	vectors[2] = -object->transform.z_vec;

	const dvec1 projectionPlaneHalfWidth = tan((glm::radians(view_angle)) / 2.0);

	projections[0] = object->transform.position + vectors[2];
	projections[1] = normalize(cross(vectors[2], vectors[1])) * projectionPlaneHalfWidth * 2.0;
	projections[2] = normalize(cross(projections[1], vectors[2])) * projectionPlaneHalfWidth * 2.0;
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

void KL::OBJECT::DATA::Camera::f_setupLenses(const CAMERA::Lens_Setup& lens) {
	switch (lens) {
		case CAMERA::Lens_Setup::ARRI_ZEISS_MASTER_PRIME_100MM: {
			lenses = {
				CAMERA::Lens( 20.00, 2.60, -15.090, 1.49702),
				CAMERA::Lens( 9.000, 2.70, -3.5400, 1.49702),

				CAMERA::Lens( 18.50, 3.05, -12.950, 1.65416),
				CAMERA::Lens(-5.675, 3.05,  11.590, 1.65416),

				CAMERA::Lens(-5.675, 3.05,  11.600, 1.60303),
				CAMERA::Lens( 13.25, 3.05, -6.0200, 1.60303),

				CAMERA::Lens(-9.325, 3.30,  16.560, 1.69104),
				CAMERA::Lens( 22.85, 3.30, -14.600, 1.69104),

				CAMERA::Lens(-5.110, 3.30,  13.410, 1.69104),
				CAMERA::Lens( 36.05, 3.30, -26.200, 1.69104),

				CAMERA::Lens( 36.05, 3.30, -26.190, 1.61344),
				CAMERA::Lens(-3.400, 3.20,  13.630, 1.61344),

				CAMERA::Lens( 3.660, 3.20,  10.880, 1.61344),
				CAMERA::Lens(-36.75, 4.00,  51.660, 1.61344),

				CAMERA::Lens( 50.80, 4.00, -35.000, 1.75704),
				CAMERA::Lens( 6.410, 4.20,  11.170, 1.75704),

				CAMERA::Lens( 10.90, 4.80,  8.5000, 1.80527),
				CAMERA::Lens( 14.00, 4.90,  6.0000, 1.80527),

				CAMERA::Lens( 24.60, 5.10, -4.5900, 1.43876),
				CAMERA::Lens( 8.180, 5.20,  13.650, 1.43876),

				CAMERA::Lens(-34.75, 5.30,  56.600, 1.61803),
				CAMERA::Lens( 15.75, 5.30,  8.0500, 1.61803),

				CAMERA::Lens( 12.45, 5.30,  11.730, 1.54075),
				CAMERA::Lens(-205.0, 5.30,  229.75, 1.54075)
			};
			break;
		}
	}
}

KL::OBJECT::DATA::CAMERA::Lens::Lens(const dvec1& curvature_radius, const dvec1& aperture_radius, const dvec1& z_distance, const dvec1& ior) :
	curvature_radius(curvature_radius),
	aperture_radius(aperture_radius),
	z_distance(z_distance),
	ior(ior)
{}