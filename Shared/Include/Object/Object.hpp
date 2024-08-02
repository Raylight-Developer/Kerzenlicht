#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "Data_Property.hpp"

#include "Nodes/Object_Nodes.hpp"

#include "Data.hpp"
#include "Shading/Material.hpp"

// FWD DECL OTHER
namespace CLASS {
	struct Scene;
	struct Data;
}
namespace SHADER{
	struct Material;
}

// FWD DECL THIS

// DECL
namespace CLASS {
	struct Object {
		string name;
		OBJECT::Data* data;
		Object* parent;
		Transform transform;
		Transform node_transform;
		vector<SHADER::Material*> materials;
		unordered_map<string, CLASS::Data> properties;

		bool viewport_visibility;
		bool render_visibility;

		CLASS::Node_Tree* node_tree;

		dmat4 transform_matrix;

		Object();

		void f_compileMatrix();
	};
}