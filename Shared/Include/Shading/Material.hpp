#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "Data_Property.hpp"

#include "Nodes/Shader_Nodes.hpp"

namespace SHADER {
	struct Material {
		string name;
		Node_Tree* node_tree;

		Material();
	};
}