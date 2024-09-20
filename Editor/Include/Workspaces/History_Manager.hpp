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
		struct Workspace_History_Manager;
		namespace HISTORY_MANAGER {
		}
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_History_Manager : Linear_Contents {
			Workspace_Manager* parent;
			List* list;

			Workspace_History_Manager(Workspace_Manager* parent);
			~Workspace_History_Manager();
		};
		namespace HISTORY_MANAGER {
		}
	}
}