#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/File.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_Log : GUI::Linear_Contents {
			Workspace_Manager* parent;
			Lace* log;

			Workspace_Log(Workspace_Manager* parent, Lace* log);
		};
	}
}