#include "Log_Console.hpp"

Log_Console::Log_Console(Window* parent) :
	GUI::Window(parent),
	parent(parent)
{
	gui_log = new GUI::Text_Stream(this);

	gui_log->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	gui_log->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
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

Log_Console& Log_Console::operator<< (const Lace& value) {
	gui_log->insertHtml(QString::fromStdString(value.str()));
	return *this;
}

void Log_Console::closeEvent(QCloseEvent* event) {
	parent->close();
	event->accept();
}