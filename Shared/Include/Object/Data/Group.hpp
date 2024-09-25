#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"


// FWD DECL OTHER
namespace KL {
	struct Object;
}

// FWD DECL THIS

// DECL
namespace KL::OBJECT::DATA {
	struct Group : Data {
		vector<Object*> objects;

		Group();
	};
}