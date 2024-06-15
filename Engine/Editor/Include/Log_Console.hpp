#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/Lace.hpp"

#define ENDL "<br>"

#define HTML_RESET   "</span>"

#define HTML_RED     "<span style='color:rgb(255, 100, 100)'>"
#define HTML_BLUE    "<span style='color:rgb(100, 100, 255)'>"
#define HTML_GREEN   "<span style='color:rgb(0  , 255, 0  )'>"
#define HTML_MAGENTA "<span style='color:rgb(255, 100, 255)'>"

struct Log_Console : GUI::Window {
	Window* parent;

	GUI::Text_Stream* gui_log;

	Log_Console(Window* parent = nullptr);

	Log_Console& operator<< (const Lace& value);

	void closeEvent(QCloseEvent* event) override;
};