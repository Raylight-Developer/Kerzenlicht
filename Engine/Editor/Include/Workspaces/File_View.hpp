#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/File.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_File_View : GUI::Linear_Contents {


			Workspace_Manager* parent;
			GUI::Text_Stream* display;

			Workspace_File_View(Workspace_Manager* parent);

			void f_refresh();
		};
	}
}