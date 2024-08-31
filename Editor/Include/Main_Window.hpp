#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"

#include "Workspaces/Manager.hpp"
#include "Workspaces/Header.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
		struct Main_Window;

		struct App : GUI::Application {
			Main_Window* window;

			App(int argc, char* argv[]);
		};

		struct Main_Window : GUI::Window {
			GUI::Application* app;
			KL::Lace* log;
			KL::Editor_File* file;
			KL::History_Manager* history;

			map<string, Workspace_Manager*> workspaces;

			bool mouse_pressed;
			bool key_pressed;

			Main_Window(GUI::Application* app);

			bool eventFilter(QObject* object, QEvent* event) override;
			void closeEvent(QCloseEvent* event) override;
		};
	}
}