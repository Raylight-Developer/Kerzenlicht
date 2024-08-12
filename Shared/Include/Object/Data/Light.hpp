#pragma once

#include "Include.hpp"

namespace KL::OBJECT::DATA {
	namespace LIGHT {
		enum struct Type {
			DIRECTIONAL,
			POINT,
			AREA,
			SPOT
		};

		struct Directional {
		};
		struct Point {
		};
		struct Area {
		};
		struct Spot {
		};
	}
	struct Light {
		LIGHT::Type type;
		void* light;

		Light();
	};
}