#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

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