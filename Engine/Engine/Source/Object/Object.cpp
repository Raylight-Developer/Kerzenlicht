#include "Object/Object.hpp"

#include "Core/File.hpp"

CLASS::Object::Object() {
	data = nullptr;
	nodes = nullptr;
	parent = nullptr;
	transform = Transform();
	transform_matrix = dmat4(1.0);
	materials = {};

	render_visibility = true;
	viewport_visibility = true;
}

void CLASS::Object::f_compileMatrix() {
	if (parent) {
		parent->f_compileMatrix();
		transform_matrix = parent->transform_matrix * transform.f_compile();
	}
	else {
		transform_matrix = transform.f_compile();
	}
}