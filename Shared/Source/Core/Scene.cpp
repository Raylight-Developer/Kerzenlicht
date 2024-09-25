#include "Core/Scene.hpp"

KL::Scene::Scene() :
	active_camera(nullptr),
	current_frame(0),
	objects({})
{}

KL::Scene::~Scene() {
}