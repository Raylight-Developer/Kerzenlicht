#pragma execution_character_set( "utf-8" )

#include "Include.hpp"
#include "Main_Window.hpp"

int main(int argc, char* argv[]) {
	SetConsoleOutputCP(65001);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_NativeWindows);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);

	GUI::WORKSPACE::App* application = new GUI::WORKSPACE::App(argc, argv);
	application->exec();
/*
	QApplication* app = new QApplication(argc, argv);
	Lace log = Lace();
	Session::getInstance().setLog(&log);

	CLASS::File file = CLASS::File::f_getAsciiFile("./Resources/Assets/Save.krz");
	Session::getInstance().setFile(&file);

	FILE->f_saveBinaryFile("./Resources/Assets/Save.krzb");
*/

	return 0;
}