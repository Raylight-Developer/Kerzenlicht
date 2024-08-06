#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "Data_Property.hpp"

#include "Nodes/Shader_Nodes.hpp"

namespace SHADER {
	struct Shader {
		string name;
		CLASS::Node_Tree* node_tree;

		Shader();
	};
}