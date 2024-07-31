#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/File.hpp"
#include "Core/Session.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Main_Window;

		struct Workspace_Header : GUI::ToolBar {
			Main_Window* parent;

			Workspace_Header(Main_Window* parent);
		};
	}
}