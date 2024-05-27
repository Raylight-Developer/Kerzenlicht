#pragma once

#include "Include.hpp"

#include "Core/Ops.hpp"
#include "Core/Node_Def.hpp"

#include "Data.hpp"
#include "Rendering/Material.hpp"

// FWD DECL OTHER
namespace CLASS {
	struct Scene;
}

// FWD DECL THIS

// DECL
namespace CLASS {
	struct Material;
	struct Object;

	struct Object {
		string name;
		OBJECT::Data* data;
		Object* parent;
		Transform transform;
		vector<Material*> materials;

		bool viewport_visibility;
		bool render_visibility;

		CLASS::Node_Tree* nodes;

		dmat4 transform_matrix;

		Object();

		void f_compileMatrix(CLASS::Scene* scene);
	};
}