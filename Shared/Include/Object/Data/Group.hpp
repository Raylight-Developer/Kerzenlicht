#pragma once

#include "Include.hpp"
namespace KL {
	struct Object;
}

namespace KL::OBJECT::DATA {
	struct Group {
		vector<Object*> objects;

		Group();
	};
}