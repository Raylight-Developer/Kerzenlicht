#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
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

	struct Force : Data  {
		FORCE::Type type;
		void* force;

		Force();
	};
}