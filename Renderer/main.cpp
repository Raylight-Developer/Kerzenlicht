#pragma execution_character_set( "utf-8" )

#include "Rendering/Renderer.hpp"

int main(int argc, char *argv[]) {
	SetConsoleOutputCP(65001);
	KL::Renderer renderer = KL::Renderer();
	renderer.f_init();
	return 0;
}