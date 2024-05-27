#pragma once

#include "Include.hpp"
namespace CLASS {
	struct Object;
}

namespace CLASS::OBJECT::DATA {
	struct Group {
		vector<Object*> objects;

		Group();
	};
}