#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/File.hpp"

#include "Main_Window.hpp"

#include "Workspaces/API.hpp"
#include "Log_Console.hpp"
#include "Workspaces/Shelf.hpp"
#include "Workspaces/Viewport.hpp"
#include "Workspaces/Properties.hpp"
#include "Workspaces/Node_Editor.hpp"
#include "Workspaces/Scene_Outliner.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Main_Window;
		struct Workspace_Dock_Header;

		enum struct Workspace_Type {
			SCENE_OUTLINER,
			NODE_EDITOR,
			PROPERTIES,
			VIEWPORT,
			SHELF,
			NONE,
			API
		};

		struct Workspace_Manager : GUI::Dock {
			Main_Window* parent;
			Log_Console* log;
			CLASS::File* file;

			GUI::Linear_Contents* workspace;
			Workspace_Dock_Header* titlebar;
			Workspace_Type type;

			Workspace_Manager(Main_Window* parent, Log_Console* log, CLASS::File* file, const Workspace_Type& type = Workspace_Type::NONE);
		};

		struct Workspace_Dock_Header : GUI::Linear_Contents {
			Workspace_Manager* parent;
			Log_Console* log;
			CLASS::File* file;

			GUI::Floating_Toggle* floating_workspace_toggle;
			GUI::Button* change_workspace_type;
			GUI::Toggle* expand_settings;
			GUI::Button* add_dock;
			GUI::Button* add_tab;
			GUI::Toggle* make_floating;
			GUI::Toggle* make_collapsible;
			GUI::Button* close_workspace;
			GUI::Menu* select_workspace_type;
			GUI::Menu* close_confirmation;
			QByteArray floating_geometry_storage;

			Workspace_Dock_Header(Workspace_Manager* parent, Log_Console* log, CLASS::File* file, const Workspace_Type& type);

			void f_initWorkspacesMenu();
			void f_selectWorkspaceTypeClick();
			void f_setWorkspace(const Workspace_Type& type = Workspace_Type::NONE);
			void f_showHideSettings(bool toggle);
			void f_initCloseConfirmationMenu();
			void f_closeClick();
			void f_closeDock();
			void f_addDockClick();
			void f_addDockTabClick();
			void f_setFloatingToggle(bool toggle);
			void f_setCollapsibleModeToggle(bool toggle);
			void f_expandCollapseToggle(bool toggle);

			void mouseDoubleClickEvent(QMouseEvent* event) override;
		};
	}
}