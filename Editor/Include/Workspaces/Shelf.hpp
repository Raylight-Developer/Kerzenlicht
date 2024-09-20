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

			Workspace_Shelf(Workspace_Manager* parent);
			~Workspace_Shelf();
		};

		struct Shelf : GUI::Tree {
			Shelf(Workspace_Shelf* parent);
			~Shelf();

			GUI::Tree_Item* objects;
			GUI::Tree_Item* object_data;
			GUI::Tree_Item* shaders;
			GUI::Tree_Item* textures;

			void f_update();

			void startDrag(Qt::DropActions actions) override;
		};
	}
}