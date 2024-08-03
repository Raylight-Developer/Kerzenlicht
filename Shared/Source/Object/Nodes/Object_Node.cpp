#include "Object/Nodes/Object_Node.hpp"

#include "Object/Nodes/Object_Nodes.hpp"

#include "Data_Property.hpp"

CLASS::Node_Tree::Node_Tree() :
	tick(nullptr)
{
	name = "#";
	nodes = {};

	references = {};
	variables = {};
}

CLASS::Node_Tree::~Node_Tree() {
	for (Node* node : nodes) delete node;
}

void CLASS::Node_Tree::exec(const dvec1* delta) const {
	if (tick) {
		tick->delta = delta;
		tick->exec();
	}
}

CLASS::Node::Node() {
	name = "#";
	type = NODE::Type::NONE;
	sub_type = 0;
}

CLASS::Node::~Node() {
	for (CLASS::NODE::Port* port : inputs) delete port;
	for (CLASS::NODE::Port* port : outputs) delete port;
}

CLASS::Data CLASS::Node::getData(const uint16& slot_id) const {
	return Data();
}

CLASS::NODE::Port::Port(Node* node) :
	node(node)
{
	type = PORT::Type::NONE;
	slot_id = 0;
}

CLASS::Data CLASS::NODE::Port::getData() const {
	return Data();
}

CLASS::NODE::PORT::Data_I_Port::Data_I_Port(Node* node, const uint16& slot_id, const DATA::Type& type, const DATA::Modifier& modifier) :
	Port(node),
	data_type(type),
	modifier(modifier)
{
	this->node = node;
	this->slot_id = slot_id;
	this->type = Type::DATA_I;
	this->connection = nullptr;
	this->default_value = Data();
}

CLASS::NODE::PORT::Data_I_Port::~Data_I_Port() {
	if (connection) {
		for (auto it = connection->outgoing_connections.begin(); it != connection->outgoing_connections.end(); ++it) {
			if (*it == this) {
				connection->outgoing_connections.erase(it);
				break;
			}
		}
		connection = nullptr;
	}
}

CLASS::Data CLASS::NODE::PORT::Data_I_Port::getData() const {
	if (connection) return connection->getData();
	return default_value;
}

CLASS::NODE::PORT::Data_O_Port::Data_O_Port(Node* node, const uint16& slot_id, const DATA::Type& type, const DATA::Modifier& modifier) :
	Port(node),
	data_type(type),
	modifier(modifier)
{
	this->node = node;
	this->slot_id = slot_id;
	this->type = Type::DATA_O;
}

CLASS::NODE::PORT::Data_O_Port::~Data_O_Port() {
	for (Data_I_Port* connection : outgoing_connections) {
		connection->connection = nullptr;
		connection = nullptr;
	}
}

CLASS::Data CLASS::NODE::PORT::Data_O_Port::getData() const {
	return node->getData(slot_id);
}

CLASS::NODE::PORT::Exec_I_Port::Exec_I_Port(Node* node, const uint16& slot_id) :
	Port(node)
{
	this->node = node;
	this->slot_id = slot_id;
	this->type = Type::EXEC_I;
}

CLASS::NODE::PORT::Exec_I_Port::~Exec_I_Port() {
	for (Exec_O_Port* connection : incoming_connections) {
		connection->connection = nullptr;
		connection = nullptr;
	}
}

void CLASS::NODE::PORT::Exec_I_Port::exec() const {
	node->exec(slot_id);
}

CLASS::NODE::PORT::Exec_O_Port::Exec_O_Port(Node* node, const uint16& slot_id) :
	Port(node)
{
	this->node = node;
	this->type = Type::EXEC_O;
	this->connection = nullptr;
}

CLASS::NODE::PORT::Exec_O_Port::~Exec_O_Port() {
	if (connection) {
		for (auto it = connection->incoming_connections.begin(); it != connection->incoming_connections.end(); ++it) {
			if (*it == this) {
				connection->incoming_connections.erase(it);
				break;
			}
		}
		connection = nullptr;
	}
}

void CLASS::NODE::PORT::Exec_O_Port::exec() const {
	if (connection) connection->exec(); // TODO can crash on recompilation of nodes
}