#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

namespace KL {
	struct Object;
	struct Scene;
}

namespace KL::OBJECT::DATA {
	struct Camera : Data {
		dvec1 view_angle;

		dvec3 projection_uv;
		dvec3 projection_u;
		dvec3 projection_v;

		Camera();

		void f_updateRayVectors(KL::Scene* scene, KL::Object* object);
		dmat4 glViewMatrix(const KL::Object* object) const;
		dmat4 glProjectionMatrix(const dvec1& aspect_ratio) const;
	};
}