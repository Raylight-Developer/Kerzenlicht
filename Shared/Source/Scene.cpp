#include "Scene.hpp"

CLASS::Scene::Scene() {
	objects = vector<CLASS::Object*>();
	active_camera = nullptr;
	current_frame = 0;
}

CLASS::Scene::~Scene() {
}