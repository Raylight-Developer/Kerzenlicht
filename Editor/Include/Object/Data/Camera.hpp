#pragma once

#include "Include.hpp"

namespace CLASS {
	struct Object;
	struct Scene;
}

namespace CLASS::OBJECT::DATA {
	struct GPU_Camera {
		dvec1 focal_length;
		dvec1 focal_angle;
		dvec1 sensor_size;

		dvec3 projection_center;
		dvec3 projection_u;
		dvec3 projection_v;

		GPU_Camera();

		void f_compile(CLASS::Scene* scene, CLASS::Object* object);
	};
}