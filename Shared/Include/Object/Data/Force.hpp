#pragma once

#include "Include.hpp"

namespace KL::OBJECT::DATA {
	namespace FORCE {
		enum struct Type {
			TURBULENCE,
			BROWNIAN,
			HARMONIC,
			MAGNETIC,
			VORTEX,
			CURVE,
			POINT,
			DRAG,
			WIND
		};
	}

	struct Force {
		FORCE::Type type;
		void* force;

		Force();
	};
}