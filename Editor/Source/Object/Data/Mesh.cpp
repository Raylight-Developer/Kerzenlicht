#include "Object/Data/Mesh.hpp"

CLASS::OBJECT::DATA::Mesh::Mesh() {
	vertices = vector<MESH::Vertex*>();
	faces = vector<MESH::Face*>();
}

CLASS::OBJECT::DATA::Mesh::~Mesh() {
	for (MESH::Vertex* vertex : vertices) delete vertex;
	for (MESH::Face* face : faces) delete face;
}