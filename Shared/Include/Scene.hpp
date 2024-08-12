#pragma once

#include "Include.hpp"

#include "Object/Object.hpp"

namespace KL {
	struct Scene {
		vector<Object*> objects;
		Object* active_camera;
		int64 current_frame;

		Scene();
		~Scene();
	};
}