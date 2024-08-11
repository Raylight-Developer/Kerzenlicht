#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Shelf;
		struct Shelf;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_Shelf : GUI::Linear_Contents {

			Workspace_Manager* parent;

			Shelf* tree;

			GUI::Tree_Item* objects;
			GUI::Tree_Item* object_data;

			Workspace_Shelf(Workspace_Manager* parent);
		};

		struct Shelf : GUI::Tree {
			Shelf(Workspace_Shelf* parent);

			void startDrag(Qt::DropActions actions) override;
		};
	}
}