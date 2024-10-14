#pragma once

#include "Core.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::OBJECT::DATA {
	namespace EMPTY {
		enum struct Type {
			POINT,
			AXES
		};
	}
	struct Empty : Data {
		Empty();
	};
}