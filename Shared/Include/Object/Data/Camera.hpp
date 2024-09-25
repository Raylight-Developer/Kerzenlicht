#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER
namespace KL {
	struct Object;
	struct Scene;
}

// FWD DECL THIS
namespace KL::OBJECT::DATA {
	struct Camera;
	namespace CAMERA {
		struct Lens;
	}
}

// DECL
namespace KL::OBJECT::DATA {
	struct Camera : Data {
		dvec1 focal_distance;
		dvec1 view_angle;
		dvec1 aperture;

		vector<CAMERA::Lens> lenses;

		Camera();

		void f_updateRayVectors(KL::Scene* scene, KL::Object* object, mat3& vectors, mat3& projections);
		dmat4 glViewMatrix(const KL::Object* object) const;
		dmat4 glProjectionMatrix(const dvec1& aspect_ratio) const;
	};
	namespace CAMERA {
		struct Lens {
			dvec1 aperture_diameter;
			dvec1 curvature_radius;
			dvec1 thickness;
			dvec1 ior;
		};
	}
}