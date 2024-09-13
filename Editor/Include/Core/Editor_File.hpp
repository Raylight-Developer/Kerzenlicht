#pragma once

#include "Shared.hpp"

#include "Nodes.hpp"

#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

#include "GUI_Nodes.hpp"
#include "GUI_Object_Nodes.hpp"

// FWD DECL OTHER
namespace KL {
	struct File;
}

// FWD DECL THIS
namespace KL {
	struct Editor_File;
}

// DECL
namespace KL {
	struct Editor_File : File {
		unordered_map<Node_Tree*, GUI::NODE::Node_Tree*> nodetree_map;
		unordered_map<Node*, GUI::NODE::Node*> node_map;

		unordered_map<string, Object*> editor_objects;
		unordered_map<string, OBJECT::Data*> editor_object_data;

		Editor_File();

		void f_loadEditorTools();

		Node_Tree* f_loadAsciiNodeTree (const Token_Array& token_data, const Tokens& line_data) override;
		void       f_loadAsciiBuild    (const Token_Array& token_data, const Tokens& line_data) override;

		void f_saveAsciiNodeTree(Lace& lace, const Node_Tree* data, const uint64& i) override;
		void f_saveBinaryNodeTree(Bin_Lace& bin, Node_Tree* data) override;
	};
}