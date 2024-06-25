#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/File.hpp"
#include "Core/Session.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_Scene_Outliner : GUI::Linear_Contents {


			Workspace_Manager* parent;

			GUI::Tree* tree;

			GUI::Tree_Item* objects;
			GUI::Tree_Item* object_data;

			Workspace_Scene_Outliner(Workspace_Manager* i_parent);
		};
	}
}