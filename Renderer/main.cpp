#include "Rendering/Renderer.hpp"

#define REALTIME_MODE
//#define RENDER_FRAME_MODE
//#define RENDER_ANIMATION_MODE

int main(int argc, char *argv[]) {
	Renderer renderer = Renderer();
	renderer.init();
	return 0;
}