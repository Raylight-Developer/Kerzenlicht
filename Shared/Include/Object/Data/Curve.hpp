#pragma once

#include "Include.hpp"

namespace KL::OBJECT::DATA {
	namespace CURVE {
		namespace SPLINE {
			enum struct Type {
				CATMULL,
				BEZIER,
				NURBS,
				POLY
			};

			struct Handle {
				dvec3 position;
			};
		}
		struct Spline {
			vector<SPLINE::Handle> handles;
			SPLINE::Type type;
		};
	}

	struct Curve {
		vector<CURVE::Spline> splines;

		Curve();
	};
}