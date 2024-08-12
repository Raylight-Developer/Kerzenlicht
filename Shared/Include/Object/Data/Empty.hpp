#pragma once

#include "Include.hpp"

namespace KL::OBJECT::DATA {
	namespace EMPTY {
		enum struct Type {
			POINT,
			AXES
		};
	}
	struct Empty {
		Empty();
	};
}