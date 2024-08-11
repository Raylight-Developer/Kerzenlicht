#pragma once

#include "Shared.hpp"

#include "Nodes.hpp"

#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

#include "GUI_Nodes.hpp"
#include "GUI_Object_Nodes.hpp"

// FWD DECL OTHER
namespace CLASS {
	struct File;
}

// FWD DECL THIS
namespace CLASS {
	struct Editor_File;
}

// DECL
namespace CLASS {
	struct Editor_File : File {
		unordered_map<CLASS::Node_Tree*, GUI::NODE::Node_Tree*> nodetree_map;
		unordered_map<CLASS::Node*, GUI::NODE::Node*> node_map;

		Editor_File();

		CLASS::Node_Tree* f_loadAsciiNodeTree   (const Token_Array& token_data) override;
		void              f_loadAsciiBuild      (const Token_Array& token_data) override;

		void f_saveAsciiNodeTree(Lace& lace, CLASS::Node_Tree* data, const uint64& i) override;
		void f_saveBinaryNodeTree(Bin_Lace& bin, Node_Tree* data) override;
	};
}