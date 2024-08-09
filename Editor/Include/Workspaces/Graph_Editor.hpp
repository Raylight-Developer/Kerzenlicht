#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/File.hpp"
#include "Core/Session.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_Grpah_Editor : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Workspace_Grpah_Editor(Workspace_Manager* parent);
		};
	}
}