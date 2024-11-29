#pragma once

#include "Core.hpp"

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
		enum struct Lens_Setup;
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

		void glProperties(KL::Scene* scene, KL::Object* object, mat3& vectors, mat3& projections) const;
		void glProperties(KL::Scene* scene, KL::Object* object, dmat3& vectors, dmat3& projections) const;
		dmat4 glViewMatrix(const KL::Object* object) const;
		dmat4 glProjectionMatrix(const dvec1& aspect_ratio) const;

		void f_setupLenses(const CAMERA::Lens_Setup& lens);
	};
	namespace CAMERA {
		struct Lens {
			dvec1 curvature_radius; // if negative, it is a convex surface (the closest point to the camera is its center)
			dvec1 aperture_radius;
			dvec1 z_distance;
			dvec1 ior;

			Lens(const dvec1& curvature_radius, const dvec1& aperture_radius, const dvec1& z_distance, const dvec1& ior);
		};
		enum struct Lens_Setup {
			NONE,
			ARRI_ZEISS_MASTER_PRIME_100MM
		};
	}
}