#include "Object/Object.hpp"

#include "Core/File.hpp"

CLASS::Object::Object() {
	nodes = nullptr;
	parent = nullptr;
	transform_matrix    = dmat4(1.0);
	materials           = vector<Material*>();
}

void CLASS::Object::f_compileMatrix(CLASS::Scene* scene) {
	if (parent) {
		parent->f_compileMatrix(scene);
		transform_matrix = parent->transform_matrix * transform.f_compile();
	}
	else {
		transform_matrix = transform.f_compile();
	}
}