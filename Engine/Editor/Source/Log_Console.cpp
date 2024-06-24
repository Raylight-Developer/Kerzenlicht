#include "Log_Console.hpp"

Log_Console::Log_Console(Window* parent) :
	GUI::Window(parent),
	parent(parent)
{
	gui_log = new GUI::Text_Stream(this);
	lace << "<pre>";

	gui_log->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	gui_log->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	gui_log->setWordWrapMode(QTextOption::NoWrap);

	gui_log->setHtml(QString::fromStdString(R"(
<style>
* {
	font-family: 'Courier New', monospace;
	font-size: medium;
	background: transparent;
	color: rgb(255,255,255);
	padding: 0;
	margin: 0;
}
</style>
)"));
	setCentralWidget(gui_log);
}

void Log_Console::closeEvent(QCloseEvent* event) {
	parent->close();
}

void Log_Console::flush() {
	lace << "</pre>";
	gui_log->insertHtml(QString::fromStdString(lace.str()));
	cout << QString::fromStdString(lace.str())
		.replace("<pre>", "")
		.replace("</pre>", "")
		.replace(ENDL, "\n")
		.replace(HTML_RESET, "")
		.replace(HTML_RED, "")
		.replace(HTML_BLUE, "")
		.replace(HTML_GREEN, "")
		.replace(HTML_MAGENTA, "")
		.toStdString();
	lace.clear();
	lace << "<pre>";
}