#include "Scene.hpp"

KL::Scene::Scene() {
	objects = vector<KL::Object*>();
	active_camera = nullptr;
	current_frame = 0;
}

KL::Scene::~Scene() {
}