#pragma once

#include "Core.hpp"

#include "Object/Object.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	struct Scene {
		vector<Object*> objects;
		Object* active_camera;
		int64 current_frame;

		Scene();
		~Scene();
	};
}