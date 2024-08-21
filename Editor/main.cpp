#pragma execution_character_set( "utf-8" )

#include "Include.hpp"
#include "Main_Window.hpp"

int main(int argc, char* argv[]) {
	SetConsoleOutputCP(65001);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_NativeWindows);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);

	GUI::WORKSPACE::App* application = new GUI::WORKSPACE::App(argc, argv);
	application->exec();

	return 0;
}