#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"
#include "Gpu_Scene.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Viewport;
		struct Viewport;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Timeline : GUI::Linear_Contents {
			Workspace_Viewport* parent;

			GUI::Label* info;
			GUI::Slider* slider;

			Timeline(Workspace_Viewport* parent);
		};

		struct Workspace_Viewport : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Viewport* viewport;
			Timeline* timeline;

			void f_systemInfo();

			Workspace_Viewport(Workspace_Manager* parent);
			~Workspace_Viewport();
		};

		struct Viewport : QWidget {
			Workspace_Viewport* parent;

			Viewport(Workspace_Viewport* parent);
		};

		class Render_Thread : public QThread {
			Q_OBJECT
		public:
			HWND hwnd;

			Render_Thread();

			void run() override;
		};
	}
}