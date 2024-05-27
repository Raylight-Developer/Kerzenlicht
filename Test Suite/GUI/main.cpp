#include "Include.hpp"

#include "Window.hpp"

#pragma execution_character_set( "utf-8" )

int main(int argc, char* argv[]) {
	SetConsoleOutputCP(65001);
	QApplication::setAttribute(Qt::ApplicationAttribute::AA_NativeWindows);

	App* app = new App(argc, argv);

	return 0;
}