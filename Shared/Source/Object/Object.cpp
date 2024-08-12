#include "Object/Object.hpp"

KL::Object::Object() :
	name("New Object"),
	node_tree(nullptr),
	parent(nullptr),
	data(nullptr)
{
	transform = Transform();
	node_transform = Transform();
	transform_matrix = dmat4(1.0);
	shaders = {};
	properties = {};

	render_visibility = true;
	viewport_visibility = true;
}

void KL::Object::f_compileMatrix() {
	if (parent) {
		parent->f_compileMatrix();
		transform_matrix = parent->transform_matrix * (transform.getMatrix() * node_transform.getMatrix());
	}
	else {
		transform_matrix = transform.getMatrix() * node_transform.getMatrix();
	}
}