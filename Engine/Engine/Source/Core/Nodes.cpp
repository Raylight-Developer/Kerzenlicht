#include "Core/Nodes.hpp"

#include "Core/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Node_GUI_Def.hpp"

CLASS::Node_Tree::Node_Tree() :
	tick(nullptr)
{
	nodes = {};

	references = {};
	variables = {};
}

CLASS::Node_Tree::Node_Tree(const GUI::NODE::Node_Tree* gui_tree) :
	tick(nullptr)
{
	unordered_map<GUI::NODE::Node*, Node*> node_map;

	for (GUI::NODE::Node* gui_node : gui_tree->nodes) {
		switch (gui_node->type) {
			case NODE::Type::EXEC: {
				switch (static_cast<NODE::EXEC::Type>(gui_node->sub_type)) {
					case NODE::EXEC::Type::COUNTER: {
						auto node = new NODE::EXEC::Counter();
						nodes.push_back(node);
						node_map[gui_node] = node;
						break;
					}
					case NODE::EXEC::Type::TICK: {
						auto node = new NODE::EXEC::Tick();
						nodes.push_back(node);
						node_map[gui_node] = node;
						tick = node;
						break;
					}
				}
				break;
			}
			case NODE::Type::LINK: {
				switch (static_cast<NODE::LINK::Type>(gui_node->sub_type)) {
					case NODE::LINK::Type::POINTER: {
						auto node = new NODE::LINK::Pointer();
						node->pointer_type = static_cast<GUI::NODE::LINK::Pointer*>(gui_node)->pointer_type;
						node->pointer = static_cast<GUI::NODE::LINK::Pointer*>(gui_node)->pointer;
						nodes.push_back(node);
						node_map[gui_node] = node;
						break;
					}
					case NODE::LINK::Type::SET: {
						switch (static_cast<GUI::NODE::LINK::Set*>(gui_node)->micro_type) {
							case NODE::LINK::SET::Type::EULER_ROTATION_X: {
								auto node = new NODE::LINK::SET::Euler_Rotation_X();
								nodes.push_back(node);
								node_map[gui_node] = node;
								break;
							}
						}
					}
				}
				break;
			}
			case NODE::Type::MATH: {
				switch (static_cast<NODE::MATH::Type>(gui_node->sub_type)) {
					case NODE::MATH::Type::ADD: {
						auto node = new NODE::MATH::Add();
						nodes.push_back(node);
						node_map[gui_node] = node;
						break;
					}
					case NODE::MATH::Type::SUB: {
						auto node = new NODE::MATH::Sub();
						nodes.push_back(node);
						node_map[gui_node] = node;
						break;
					}
					case NODE::MATH::Type::MUL: {
						auto node = new NODE::MATH::Mul();
						nodes.push_back(node);
						node_map[gui_node] = node;
						break;
					}
					case NODE::MATH::Type::DIV: {
						auto node = new NODE::MATH::Div();
						nodes.push_back(node);
						node_map[gui_node] = node;
						break;
					}
				}
				break;
			}
		}
	}
	//Connections
	for (GUI::NODE::Node* gui_node : gui_tree->nodes) {
		for (GUI::NODE::Port* port : gui_node->inputs) {
			if (port->type == NODE::PORT::Type::DATA_I) {
				auto cast_port = static_cast<GUI::NODE::PORT::Data_I_Port*>(port);
				if (cast_port->connection) {
					for (auto port_r : node_map[gui_node]->inputs) {
						if (port_r->slot_id == port->slot_id) {
							for (auto port_l : node_map[cast_port->connection->port_l->node]->outputs) {
								if (port_l->slot_id == cast_port->connection->port_l->slot_id) {
									static_cast<NODE::PORT::Data_I_Port*>(port_r)->connection = static_cast<NODE::PORT::Data_O_Port*>(port_l);
									cout << "Connect Data L_Node[" << getKeyByValue(node_map, port_l->node)->label.toStdString() << "] : " << port_l->slot_id << " To R_Node[" << gui_node->label.toStdString() << "] : " << port_r->slot_id << endl;
								}
							}
						}
					}
				}
			}
		}
		for (GUI::NODE::Port* port : gui_node->outputs) {
			if (port->type == NODE::PORT::Type::EXEC_O) {
				auto cast_port = static_cast<GUI::NODE::PORT::Exec_O_Port*>(port);
				if (cast_port->connection) {
					for (auto port_l : node_map[gui_node]->outputs) {
						if (port_l->slot_id == port->slot_id) {
							for (auto port_r : node_map[cast_port->connection->port_r->node]->inputs) {
								if (port_r->slot_id == cast_port->connection->port_r->slot_id) {
									static_cast<NODE::PORT::Exec_O_Port*>(port_l)->connection = static_cast<NODE::PORT::Exec_I_Port*>(port_r);
									cout << "Connect Exec L_Node[" << gui_node->label.toStdString() << "] : " << port_l->slot_id << " To R_Node[" << getKeyByValue(node_map, port_r->node)->label.toStdString() << "] : " << port_r->slot_id << endl;
								}
							}
						}
					}
				}
			}
		}
	}
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
	type = NODE::Type::NONE;
	sub_type = 0;
}

CLASS::Node::~Node() {
	for (CLASS::NODE::Port* port : inputs) delete port;
	for (CLASS::NODE::Port* port : outputs) delete port;
}

CLASS::NODE::Data CLASS::Node::getData(const uint16& slot_id) const {
	return NODE::Data();
}

CLASS::NODE::Data::Data() {
	data = nullptr;
	type = DATA::Type::NONE;
	modifier = DATA::Modifier::SINGLE;
}

CLASS::NODE::Data::Data(const any& data, const DATA::Type& type, const DATA::Modifier& modifier) :
	data(data),
	type(type),
	modifier(modifier)
{}

CLASS::NODE::Data CLASS::NODE::Data::operator+(const Data & other) {
	if (type == other.type) {
		switch (type) {
			case DATA::Type::DOUBLE: return Data(any_cast<dvec1>(data) + any_cast<dvec1>(other.data), type);
		}
	}
	return Data();
}
CLASS::NODE::Data CLASS::NODE::Data::operator-(const Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return Data(any_cast<dvec1>(data) - any_cast<dvec1>(other.data), type);
		}
	}
	return Data();
}

CLASS::NODE::Data CLASS::NODE::Data::operator*(const Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return Data(any_cast<dvec1>(data) * any_cast<dvec1>(other.data), type);
		}
	}
	return Data();
}

CLASS::NODE::Data CLASS::NODE::Data::operator/(const Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return Data(any_cast<dvec1>(data) / any_cast<dvec1>(other.data), type);
		}
	}
	return Data();
}


CLASS::NODE::Port::Port(Node* node) :
	node(node)
{
	type = PORT::Type::NONE;
	slot_id = 0;
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

CLASS::NODE::Data CLASS::NODE::PORT::Data_I_Port::getData() const {
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

CLASS::NODE::Data CLASS::NODE::PORT::Data_O_Port::getData() const {
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
	if (connection) connection->exec();
}

QColor typeColor(const CLASS::NODE::DATA::Type& type) {
	switch (type) {
		case CLASS::NODE::DATA::Type::NONE:  return QColor(  0,  0,  0);
		case CLASS::NODE::DATA::Type::ANY:    return QColor(150,150,150);
		case CLASS::NODE::DATA::Type::STRING: return QColor(215,155,135);
		case CLASS::NODE::DATA::Type::DOUBLE: return QColor( 95,230, 95);
		case CLASS::NODE::DATA::Type::BOOL:   return QColor(240,100,175);
		case CLASS::NODE::DATA::Type::UINT:   return QColor(105,125, 60);
		case CLASS::NODE::DATA::Type::INT:    return QColor( 40,130, 40);

		case CLASS::NODE::DATA::Type::TRANSFORM: return QColor( 85, 85,240);
		case CLASS::NODE::DATA::Type::TEXTURE:   return QColor(240, 85, 85);
		case CLASS::NODE::DATA::Type::OBJECT:    return QColor(250,175,100);
		case CLASS::NODE::DATA::Type::SCENE:     return QColor( 85,195,240);
		case CLASS::NODE::DATA::Type::DATA:      return QColor(210,240, 85);

		case CLASS::NODE::DATA::Type::VEC2:
		case CLASS::NODE::DATA::Type::VEC3:
		case CLASS::NODE::DATA::Type::VEC4:
		case CLASS::NODE::DATA::Type::IVEC2:
		case CLASS::NODE::DATA::Type::IVEC3:
		case CLASS::NODE::DATA::Type::IVEC4:
		case CLASS::NODE::DATA::Type::UVEC2:
		case CLASS::NODE::DATA::Type::UVEC3:
		case CLASS::NODE::DATA::Type::UVEC4: return QColor(165, 110, 230);

		case CLASS::NODE::DATA::Type::MAT2:
		case CLASS::NODE::DATA::Type::MAT3:
		case CLASS::NODE::DATA::Type::MAT4:
		case CLASS::NODE::DATA::Type::IMAT2:
		case CLASS::NODE::DATA::Type::IMAT3:
		case CLASS::NODE::DATA::Type::IMAT4:
		case CLASS::NODE::DATA::Type::UMAT2:
		case CLASS::NODE::DATA::Type::UMAT3:
		case CLASS::NODE::DATA::Type::UMAT4: return QColor(230, 180, 240);
	}
	return QColor(0, 0, 0);
}