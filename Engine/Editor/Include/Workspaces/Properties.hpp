#pragma once

#include "Include.hpp"
#include "QT.hpp"
#include "GUI.hpp"

#include "Core/File.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_Properties : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Workspace_Properties(Workspace_Manager* i_parent);
		};

		struct Object_Properties : GUI::Linear_Contents {
			Workspace_Properties* parent;
			CLASS::File* file;

			Object_Properties(Workspace_Properties* i_parent);
		};
	}
}