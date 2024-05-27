#pragma once

#include "Include.hpp"

namespace CLASS::OBJECT::DATA {
	namespace PRIMITIVE {
		enum struct Type {
			ELLIPSOID,
			CYLINDER,
			CAPSULE,
			SPHERE,
			TORUS,
			BOX
		};

		struct Ellipsoid {
			dvec3 size;
		};
		struct Cylinder {
			dvec3 pos_a;
			dvec1 radius_a;
			dvec3 pos_b;
			dvec1 radius_b;
		};
		struct Capsule {
			dvec3 pos_a;
			dvec1 radius_a;
			dvec3 pos_b;
			dvec1 radius_b;
		};
		struct Sphere {
			dvec1 radius;
		};
		struct Torus {
			dvec1 inner_radius;
			dvec1 outer_radius;
		};
		struct Box {
			dvec3 size;
		};
	}

	struct Primitive {
		PRIMITIVE::Type type;
		void* primitive;
	};
}