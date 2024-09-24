#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

namespace KL {
	struct Object;
	struct Scene;
}

namespace KL::OBJECT::DATA {
	struct Camera : Data {
		dvec1 focal_distance;
		dvec1 view_angle;
		dvec1 aperture;

		Camera();

		void f_updateRayVectors(KL::Scene* scene, KL::Object* object, mat3& vectors, mat3& projections);
		dmat4 glViewMatrix(const KL::Object* object) const;
		dmat4 glProjectionMatrix(const dvec1& aspect_ratio) const;
	};
}