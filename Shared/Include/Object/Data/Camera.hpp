#pragma once

#include "Include.hpp"

namespace KL {
	struct Object;
	struct Scene;
}

namespace KL::OBJECT::DATA {
	struct Camera {
		dvec1 view_angle;

		dvec3 projection_uv;
		dvec3 projection_u;
		dvec3 projection_v;

		Camera();

		void compile(KL::Scene* scene, KL::Object* object, const dvec1& aspect_ratio);
		dmat4 glViewMatrix(const KL::Object* object) const;
		dmat4 glProjectionMatrix(const dvec1& aspect_ratio) const;
	};
}