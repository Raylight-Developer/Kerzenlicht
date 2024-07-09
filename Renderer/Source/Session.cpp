#include "Session.hpp"

#include "Rendering/Gpu_Scene.hpp"

Session& Session::getInstance() {
	static Session instance;
	return instance;
}

void Session::setScene(GPU_Scene* ptr) {
	scene = ptr;
}

GPU_Scene* Session::getScene() {
	return scene;
}