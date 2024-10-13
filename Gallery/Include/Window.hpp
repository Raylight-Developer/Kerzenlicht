#pragma once

#include "Database.hpp"

#include "QT.hpp"

namespace GUI {
	struct Main_Window;
	struct App;
}

namespace GUI {
	struct App : Application {
		Main_Window* window;

		App(int argc, char* argv[]);
	};

	struct Main_Window : Window {
		Application* app;
		KL::Lace* log;

		Main_Window(GUI::Application* app);

		void closeEvent(QCloseEvent* event) override;
	};
}