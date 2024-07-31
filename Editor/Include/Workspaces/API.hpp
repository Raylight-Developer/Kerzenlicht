#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/File.hpp"
#include "Core/Session.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_API : GUI::Linear_Contents {
			Workspace_Manager* parent;

			GUI::Text_Stream* console_output;
			GUI::Text_Stream* console_output_history;
			uint32 history_counter;

			Workspace_API(Workspace_Manager* i_parent);

			void f_editCode();
			void f_executeCode();

			void keyPressEvent(QKeyEvent* P_Event) override;
		};
	}
}