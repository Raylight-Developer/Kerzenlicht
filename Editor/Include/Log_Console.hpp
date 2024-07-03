#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Lace.hpp"

#define ENDL "<br>"

#define HTML_RESET   "</span>"

#define HTML_RED     "<span style='color:rgb(255, 100, 100)'>"
#define HTML_BLUE    "<span style='color:rgb(100, 200, 255)'>"
#define HTML_GREEN   "<span style='color:rgb(0  , 255, 0  )'>"
#define HTML_MAGENTA "<span style='color:rgb(255, 100, 255)'>"

struct Log_Console : GUI::Window { // TODO separate for editor and non-QT runtime
	Window* parent;

	GUI::Text_Stream* gui_log;
	Lace lace;

	Log_Console(Window* parent = nullptr);

	void closeEvent(QCloseEvent* event) override;

	void flush();

	template <typename T>
	Log_Console& operator<<(const T& value) {
		lace << value;
		gui_log->verticalScrollBar()->setValue(gui_log->verticalScrollBar()->maximum());
		return *this;
	};
};