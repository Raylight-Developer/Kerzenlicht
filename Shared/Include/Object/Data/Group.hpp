#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

namespace KL {
	struct Object;
}

namespace KL::OBJECT::DATA {
	struct Group : Data {
		vector<Object*> objects;

		Group();
	};
}