#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "Data_Property.hpp"

#include "Nodes/Object_Nodes.hpp"

#include "Data.hpp"
#include "Shader/Shader.hpp"

// FWD DECL OTHER
namespace KL {
	struct Scene;
	struct Data;
}
namespace SHADER {
	struct Shader;
}

// FWD DECL THIS

// DECL
namespace KL {
	struct Object {
		string name;
		OBJECT::Data* data;
		Object* parent;
		Transform transform;
		Transform node_transform;
		vector<KL::Shader*> shaders;
		unordered_map<string, KL::Data> properties;

		bool viewport_visibility;
		bool render_visibility;

		KL::Node_Tree* node_tree;

		dmat4 transform_matrix;

		Object();

		void f_compileMatrix();
	};
}