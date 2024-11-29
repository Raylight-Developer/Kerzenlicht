#pragma once

#include "Core.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::OBJECT::DATA {
	namespace CURVE {
		namespace SPLINE {
			enum struct Type {
				NONE,
				CATMULL,
				BEZIER,
				NURBS,
				POLY
			};
		}
		struct Spline {
			vector<dvec3> points;
			SPLINE::Type type;

			Spline();
		};
	}

	struct Curve : Data  {
		vector<CURVE::Spline*> splines;
		bool cpu_update;

		Curve();
	};
}