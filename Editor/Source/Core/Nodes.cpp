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
						auto t_node = new NODE::EXEC::Script(static_cast<GUI::NODE::EXEC::Script*>(gui_node)->script_identifier->text().toStdString());
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
			if (port->type == NODE::PORT::Type::DATA_I) { // TODO nullptr? Crash
				auto cast_port = static_cast<GUI::NODE::PORT::Data_I_Port*>(port);
				if (cast_port->connection) {
					for (auto port_r : node_map[gui_node]->inputs) {
						if (port_r->slot_id == port->slot_id) {
							for (auto port_l : node_map[cast_port->connection->port_l->node]->outputs) {
								if (port_l->slot_id == cast_port->connection->port_l->slot_id) {
									static_cast<NODE::PORT::Data_I_Port*>(port_r)->connection = static_cast<NODE::PORT::Data_O_Port*>(port_l);
#									ifdef LOG0
										cout << endl << "Connect Data L_Node[" << getKeyByValue(node_map, port_l->node)->label.toStdString() << "] : " << port_l->slot_id << " To R_Node[" << gui_node->label.toStdString() << "] : " << port_r->slot_id;
									#endif
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
									#ifdef LOG0
										cout << endl << "Connect Exec L_Node[" << gui_node->label.toStdString() << "] : " << port_l->slot_id << " To R_Node[" << getKeyByValue(node_map, port_r->node)->label.toStdString() << "] : " << port_r->slot_id;
									#endif
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


CLASS::Node_Tree* CLASS::File::f_loadNodeTree(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	auto node_tree = new CLASS::Node_Tree();
	auto gui_node_tree = new GUI::NODE::Node_Tree();

	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "┌Node") {
			is_processing = true;
			read_data.clear();
			read_data.push_back(tokens);
		}
		else if (tokens[0] == "└Node") {
			is_processing = false;
			CLASS::Node* node = nullptr;
			GUI::NODE::Node* gui_node = nullptr;
			if      (read_data[0][2] == "CONSTRAINT") {
			}
			else if (read_data[0][2] == "GENERATE") {
			}
			else if (read_data[0][2] == "PHYSICS") {
			}
			else if (read_data[0][2] == "MODIFY") {
			}
			else if (read_data[0][2] == "EXEC") {
				if (read_data[0][4] == "SCRIPT") {
					node = new NODE::EXEC::Script(f_join(read_data[3]));
					gui_node = new GUI::NODE::EXEC::Script(str_to_i(read_data[2][1], read_data[2][2]), f_join(read_data[3]));
				}
				else if (read_data[0][4] == "TICK") {
					node = new NODE::EXEC::Tick();
					gui_node = new GUI::NODE::EXEC::Tick(str_to_i(read_data[2][1], read_data[2][2]));
					node_tree->tick = static_cast<NODE::EXEC::Tick*>(node);
				}
			}
			else if (read_data[0][2] == "LINK") {
				if      (read_data[0][4] == "POINTER") {
					auto pointer = new NODE::LINK::Pointer();
					pointer->pointer_type = static_cast<NODE::DATA::Type>(str_to_u(read_data[3][1]));
					node = pointer;
					gui_node = new GUI::NODE::LINK::Pointer(str_to_i(read_data[2][1], read_data[2][2]), static_cast<NODE::DATA::Type>(str_to_u(read_data[3][1])));
				}
				else if (read_data[0][4] == "GET") {
					if      (read_data[0][6] == "FIELD") {
						auto field = new NODE::LINK::GET::Field();
						field->field = f_join(read_data[3]);
						node = field;
						auto gui_field = new GUI::NODE::LINK::GET::Field(str_to_i(read_data[2][1], read_data[2][2]));
						gui_node = gui_field;
						gui_field->field->setText(QString::fromStdString(f_join(read_data[3])));
					}
				}
				else if (read_data[0][4] == "SET") {
					if      (read_data[0][6] == "TRANSFORM") {
						if      (read_data[0][8] == "EULER_ROTATION") {
							if      (read_data[0][10] == "X") {
								node = new NODE::LINK::SET::Euler_Rotation_X();
								gui_node = new GUI::NODE::LINK::SET::Euler_Rotation_X(str_to_i(read_data[2][1], read_data[2][2]));
							}
						}
					}
				}
			}
			else if (read_data[0][2] == "MATH") {
				if      (read_data[0][4] == "ADD") {
					node = new NODE::MATH::Add();
					gui_node = new GUI::NODE::MATH::Add(str_to_i(read_data[2][1], read_data[2][2]));
				}
				else if (read_data[0][4] == "SUB") {
					node = new NODE::MATH::Sub();
					gui_node = new GUI::NODE::MATH::Sub(str_to_i(read_data[2][1], read_data[2][2]));
				}
				else if (read_data[0][4] == "MUL") {
					node = new NODE::MATH::Mul();
					gui_node = new GUI::NODE::MATH::Mul(str_to_i(read_data[2][1], read_data[2][2]));
				}
				else if (read_data[0][4] == "DIV") {
					node = new NODE::MATH::Div();
					gui_node = new GUI::NODE::MATH::Div(str_to_i(read_data[2][1], read_data[2][2]));
				}
			}
			else if (read_data[0][2] == "UTIL") {
				if (read_data[0][4] == "PRINT") {
					node = new NODE::UTIL::Print();
					gui_node = new GUI::NODE::UTIL::Print(str_to_i(read_data[2][1], read_data[2][2]));
				}
			}
			else {
				// TODO load "unkown" node from newer version
			}

			pointer_map[str_to_ul(read_data[1][1])] = node;
			gui_node_tree->nodes.push_back(gui_node);
			node_tree->nodes.push_back(node);
			node_map[node] = gui_node;
		}
		else if (tokens[0] == "┌Build-E") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Build-E") {
			*LOG << ENDL << "    Loading Build-E..."; FLUSH
				is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto port_l = static_cast<NODE::PORT::Exec_O_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])->outputs[str_to_ul(sub_tokens[2])]
					);
				auto port_r = static_cast<NODE::PORT::Exec_I_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])->inputs[str_to_ul(sub_tokens[3])]
					);
				port_l->connection = port_r;
				port_r->incoming_connections.push_back(port_l);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Exec_O_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])]->outputs[str_to_ul(sub_tokens[2])]
					);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Exec_I_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])]->inputs[str_to_ul(sub_tokens[3])]
					);
				gui_port_l->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_r->incoming_connections.push_back(gui_port_l->connection);
			}
		}
		else if (tokens[0] == "┌Build-D") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Build-D") {
			*LOG << ENDL << "    Loading Build-D..."; FLUSH
				is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto port_l = static_cast<NODE::PORT::Data_O_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])->outputs[str_to_ul(sub_tokens[2])]
					);
				auto port_r = static_cast<NODE::PORT::Data_I_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])->inputs[str_to_ul(sub_tokens[3])]
					);
				port_r->connection = port_l;
				port_l->outgoing_connections.push_back(port_r);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Data_O_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])]->outputs[str_to_ul(sub_tokens[2])]
					);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Data_I_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])]->inputs[str_to_ul(sub_tokens[3])]
					);
				gui_port_r->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_l->outgoing_connections.push_back(gui_port_r->connection);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	this->nodetree_map[node_tree] = gui_node_tree;
	pointer_map[str_to_ul(token_data[1][1])] = node_tree;
	return node_tree;
}

#define NL << NL() <<
#define SP << S() <<
#define A += 1;
#define R -= 1;

void CLASS::File::f_saveNodeTree(Lace& lace, CLASS::Node_Tree* data, const uint64& i) {
	uint64 j = 0;
	lace NL "┌Node-Tree [ " << i << " ]";
	lace A
		lace NL ptr_to_str(data);
	lace NL "┌Nodes( " << data->nodes.size() << " )";
	lace A
		for (CLASS::Node* node : data->nodes) {
			switch (node->type) {
			case NODE::Type::CONSTRAINT: {
				switch (static_cast<NODE::CONSTRAINT::Type>(node->sub_type)) {
				case NODE::CONSTRAINT::Type::PARENT: {
					break;
				}
				}
				break;
			}
			case NODE::Type::EXEC: {
				switch (static_cast<NODE::EXEC::Type>(node->sub_type)) {
				case NODE::EXEC::Type::COUNTER: {
					lace NL "┌Node :: EXEC :: COUNTER [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::EXEC::Type::SCRIPT: {
					lace NL "┌Node :: EXEC :: SCRIPT [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL S() << static_cast<CLASS::NODE::EXEC::Script*>(node)->script_id;
					lace NL "└Node";
					break;
				}
				case NODE::EXEC::Type::TICK: {
					lace NL "┌Node :: EXEC :: TICK [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				}
				break;
			}
			case NODE::Type::LINK: {
				switch (static_cast<NODE::LINK::Type>(node->sub_type)) {
				case NODE::LINK::Type::POINTER: {
					lace NL "┌Node :: LINK :: POINTER [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL S() << "Type " << e_to_u(static_cast<NODE::LINK::Pointer*>(node)->pointer_type);
					lace NL "└Node";
					break;
				}
				case NODE::LINK::Type::GET: {
					switch (static_cast<CLASS::NODE::LINK::Get*>(node)->mini_type) {
					case NODE::LINK::GET::Type::FIELD: {
						lace NL "┌Node :: LINK :: GET :: FIELD [ " << j++ << " ]";
						lace NL S() << ptr_to_str(node);
						lace NL S() << "Pos " << node_map[node]->scenePos();
						lace NL S() << static_cast<NODE::LINK::GET::Field*>(node)->field;
						lace NL "└Node";
						break;
					}
					}
					break;
				}
				case NODE::LINK::Type::SET: {
					switch (static_cast<CLASS::NODE::LINK::Set*>(node)->mini_type) {
					case NODE::LINK::SET::Type::EULER_ROTATION_X: {
						lace NL "┌Node :: LINK :: SET :: TRANSFORM :: EULER_ROTATION :: X [ " << j++ << " ]";
						lace NL S() << ptr_to_str(node);
						lace NL S() << "Pos " << node_map[node]->scenePos();
						lace NL "└Node";
						break;
					}
					}
					break;
				}
				}
				break;
			}
			case NODE::Type::MATH: {
				switch (static_cast<NODE::MATH::Type>(node->sub_type)) {
				case NODE::MATH::Type::ADD: {
					lace NL "┌Node :: MATH :: ADD [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::MATH::Type::SUB: {
					lace NL "┌Node :: MATH :: SUB [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::MATH::Type::MUL: {
					lace NL "┌Node :: MATH :: MUL [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::MATH::Type::DIV: {
					lace NL "┌Node :: MATH :: DIV [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				}
				break;
			}
			case NODE::Type::UTIL: {
				switch (static_cast<NODE::UTIL::Type>(node->sub_type)) {
				case NODE::UTIL::Type::PRINT: {
					lace NL "┌Node :: UTIL :: PRINT [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::UTIL::Type::VIEW: {
					lace NL "┌Node :: UTIL :: VIEW [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				}
				break;
			}
			}
		}
	lace R
		lace NL "└Nodes";
	lace NL "┌Build-E"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace A
		for (const auto node_l : data->nodes) {
			for (const auto& port : node_l->outputs) {
				if (port->type == NODE::PORT::Type::EXEC_O) {
					const auto port_l = static_cast<CLASS::NODE::PORT::Exec_O_Port*>(port);
					if (port_l->connection) {
						const auto port_r = port_l->connection;
						const auto node_r = port_r->node;
						const uint64 port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
						const uint64 port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
						//const uint64 node_r_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_r));
						//lace NL node_l_id SP port_l_id SP node_r_id SP port_r_id;
						lace NL ptr_to_str(node_l) SP port_l_id SP port_r_id SP ptr_to_str(node_r);
					}
				}
			}
		}
	lace R
		lace NL "└Build-E";
	lace NL "┌Build-D"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace A
		for (const auto node_r : data->nodes) {
			for (const auto& port : node_r->inputs) {
				if (port->type == NODE::PORT::Type::DATA_I) {
					const auto port_r = static_cast<CLASS::NODE::PORT::Data_I_Port*>(port);
					if (port_r->connection) {
						const auto port_l = port_r->connection;
						const auto node_l = port_l->node;
						const uint64 port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
						const uint64 port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
						//const uint64 node_l_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_l));
						//lace NL node_l_id SP port_l_id SP node_r_id SP port_r_id;
						lace NL ptr_to_str(node_l) SP port_l_id SP port_r_id SP ptr_to_str(node_r);
					}
				}
			}
		}
	lace R
		lace NL "└Build-D";
	lace R
		lace NL "└Node-Tree";
}