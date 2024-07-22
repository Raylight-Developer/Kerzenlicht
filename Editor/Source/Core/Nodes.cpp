#include "Core/Nodes.hpp"

#include "Core/Node_Def.hpp"
#include "Core/File.hpp"

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
		Node* node = nullptr;
		switch (gui_node->type) {
			case NODE::Type::EXEC: {
				switch (static_cast<NODE::EXEC::Type>(gui_node->sub_type)) {
					case NODE::EXEC::Type::SCRIPT: {
						auto t_node = new NODE::EXEC::Script();
						t_node->script_id = static_cast<GUI::NODE::EXEC::Script*>(gui_node)->script_identifier->text().toStdString();
						node = t_node;
						break;
					}
					case NODE::EXEC::Type::COUNTER: {
						auto t_node = new NODE::EXEC::Counter();
						node = t_node;
						break;
					}
					case NODE::EXEC::Type::TICK: {
						auto t_node = new NODE::EXEC::Tick();
						tick = t_node;
						node = t_node;
						break;
					}
				}
				break;
			}
			case NODE::Type::LINK: {
				switch (static_cast<NODE::LINK::Type>(gui_node->sub_type)) {
					case NODE::LINK::Type::POINTER: {
						auto t_node = new NODE::LINK::Pointer();
						t_node->pointer_type = static_cast<GUI::NODE::LINK::Pointer*>(gui_node)->pointer_type;
						t_node->pointer = static_cast<GUI::NODE::LINK::Pointer*>(gui_node)->pointer;
						node = t_node;
						break;
					}
					case NODE::LINK::Type::GET: {
						switch (static_cast<GUI::NODE::LINK::Get*>(gui_node)->mini_type) {
							case NODE::LINK::GET::Type::FIELD: {
								auto t_node = new NODE::LINK::GET::Field();
								t_node->field = static_cast<GUI::NODE::LINK::GET::Field*>(gui_node)->field->text().toStdString();
								node = t_node;
								break;
							}
						}
						break;
					}
					case NODE::LINK::Type::SET: {
						switch (static_cast<GUI::NODE::LINK::Set*>(gui_node)->mini_type) {
							case NODE::LINK::SET::Type::EULER_ROTATION_X: {
								auto t_node = new NODE::LINK::SET::Euler_Rotation_X();
								node = t_node;
								break;
							}
						}
						break;
					}
				}
				break;
			}
			case NODE::Type::MATH: {
				switch (static_cast<NODE::MATH::Type>(gui_node->sub_type)) {
					case NODE::MATH::Type::ADD: {
						auto t_node = new NODE::MATH::Add();
						node = t_node;
						break;
					}
					case NODE::MATH::Type::SUB: {
						auto t_node = new NODE::MATH::Sub();
						node = t_node;
						break;
					}
					case NODE::MATH::Type::MUL: {
						auto t_node = new NODE::MATH::Mul();
						node = t_node;
						break;
					}
					case NODE::MATH::Type::DIV: {
						auto t_node = new NODE::MATH::Div();
						node = t_node;
						break;
					}
				}
				break;
			}
			case NODE::Type::UTIL: {
				switch (static_cast<NODE::UTIL::Type>(gui_node->sub_type)) {
					case NODE::UTIL::Type::PRINT: {
						auto t_node = new NODE::UTIL::Print();
						node = t_node;
						break;
					}
				}
				break;
			}
		}
		nodes.push_back(node);
		node_map[gui_node] = node;
		FILE->node_map[node] = gui_node;
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
									cout << endl << "Connect Data L_Node[" << getKeyByValue(node_map, port_l->node)->label.toStdString() << "] : " << port_l->slot_id << " To R_Node[" << gui_node->label.toStdString() << "] : " << port_r->slot_id;
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
									cout << endl << "Connect Exec L_Node[" << gui_node->label.toStdString() << "] : " << port_l->slot_id << " To R_Node[" << getKeyByValue(node_map, port_r->node)->label.toStdString() << "] : " << port_r->slot_id;
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

CLASS::NODE::Data::Data(const string& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::STRING;
}
CLASS::NODE::Data::Data(const dvec1& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::DOUBLE;
}
CLASS::NODE::Data::Data(const bool& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::BOOL;
}
CLASS::NODE::Data::Data(const uint64& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::UINT;
}
CLASS::NODE::Data::Data(const int64& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::INT;
}

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

uint64 CLASS::NODE::Data::getUint() const {
	return any_cast<uint64>(data);
}

dvec1 CLASS::NODE::Data::getDouble() const {
	switch (type) {
		case DATA::Type::DOUBLE: return any_cast<dvec1>(data);
		case DATA::Type::UINT: return static_cast<dvec1>(any_cast<uint64>(data));
		case DATA::Type::INT: return static_cast<dvec1>(any_cast<int64>(data));
	}
	return 0.0;
}

CLASS::Scene* CLASS::NODE::Data::getScene() const {
	return any_cast<CLASS::Scene*>(data);
}

CLASS::Object* CLASS::NODE::Data::getObject() const {
	return any_cast<CLASS::Object*>(data);
}

string CLASS::NODE::Data::to_string() const {
	switch (type) {
		case DATA::Type::TRANSFORM: return any_cast<Transform>(data).to_string();
		case DATA::Type::DOUBLE:    return std::to_string(any_cast<dvec1>(data));
		case DATA::Type::UINT:      return std::to_string(static_cast<dvec1>(any_cast<uint64>(data)));
		case DATA::Type::INT:       return std::to_string(static_cast<dvec1>(any_cast<int64>(data)));
	}
	return "";
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
	if (connection) connection->exec(); // TODO can crash on recompilation of nodes
}

QColor typeColor(const CLASS::NODE::DATA::Type& type) {
	switch (type) {
		case CLASS::NODE::DATA::Type::NONE:   return QColor(  0,   0,   0);
		case CLASS::NODE::DATA::Type::ANY:    return QColor(150, 150, 150);
		case CLASS::NODE::DATA::Type::STRING: return QColor(215, 155, 135);
		case CLASS::NODE::DATA::Type::DOUBLE: return QColor( 95, 230,  95);
		case CLASS::NODE::DATA::Type::BOOL:   return QColor(240, 100, 175);
		case CLASS::NODE::DATA::Type::UINT:   return QColor(105, 125,  60);
		case CLASS::NODE::DATA::Type::INT:    return QColor( 40, 130,  40);

		case CLASS::NODE::DATA::Type::TRANSFORM: return QColor( 85,  85, 240);
		case CLASS::NODE::DATA::Type::TEXTURE:   return QColor(240,  85,  85);
		case CLASS::NODE::DATA::Type::OBJECT:    return QColor(250, 175, 100);
		case CLASS::NODE::DATA::Type::SCENE:     return QColor( 85, 195, 240);
		case CLASS::NODE::DATA::Type::DATA:      return QColor(210, 240,  85);

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