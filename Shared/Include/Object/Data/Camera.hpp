#pragma once

#include "Include.hpp"

namespace KL {
	struct Object;
	struct Scene;
}

namespace KL::OBJECT::DATA {
	struct Camera {
		dvec1 focal_length;
		dvec1 focal_angle;
		dvec1 sensor_size;

		dvec3 projection_uv;
		dvec3 projection_u;
		dvec3 projection_v;

		Camera();

		void compile(KL::Scene* scene, KL::Object* object);
		void updateFocalAngle(); // TODO Make it Accurate
		mat4 glViewMatrix(const KL::Object* object) const;
		mat4 glProjectionMatrix(const dvec1& aspect_ratio) const;
	};
}