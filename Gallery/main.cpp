#pragma execution_character_set( "utf-8" )

#include "Window.hpp"

int main(int argc, char* argv[]) {
	SetConsoleOutputCP(65001);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_NativeWindows);

	GUI::App* application = new GUI::App(argc, argv);
	application->exec();

	return 0;
}