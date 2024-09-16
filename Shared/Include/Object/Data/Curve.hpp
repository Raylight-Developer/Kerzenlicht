#pragma once

#include "Include.hpp"

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

	struct Curve {
		vector<CURVE::Spline*> splines;
		bool cpu_update;

		Curve();
	};
}