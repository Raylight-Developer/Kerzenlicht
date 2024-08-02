#include "GUI_Object_Nodes.hpp"

#include "Object/Object.hpp"

GUI::NODE::EXEC::Counter::Counter(const ivec2& pos) {
	label = "Tick Counter";
	type = CLASS::NODE::Type::EXEC;
	sub_type = e_to_u(CLASS::NODE::EXEC::Type::COUNTER);

	rect = QRectF(-100, -20, 200, 40);

	i_exec  = new PORT::Exec_I_Port(this, 0, "Tick");

	o_exec  = new PORT::Exec_O_Port(this, 0, "Tick");
	o_count = new PORT::Data_O_Port(this, 1, "Count", CLASS::DATA::Type::UINT);

	inputs.push_back(i_exec);
	outputs.push_back(o_exec);
	outputs.push_back(o_count);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}

GUI::NODE::EXEC::Script::Script(const ivec2& pos, const string& script_id) {
	label = "Script";
	type = CLASS::NODE::Type::EXEC;
	sub_type = e_to_u(CLASS::NODE::EXEC::Type::SCRIPT);

	wrapper = new Script_Node(this);
	buildGuiFunc = nullptr;
	paintGuiFunc = nullptr;
	dynlib = NULL;

	rect = QRectF(-100, -20, 200, 60);
	load_pos = QPointF(pos.x, pos.y);

	script_identifier_proxyWidget = new QGraphicsProxyWidget(this);
	script_identifier = new GUI::Value_Input();
	script_identifier->setFixedSize(90, 20);
	script_identifier->setPlaceholderText("Script_ID");
	script_identifier->setText(QString::fromStdString(script_id));
	script_identifier_proxyWidget->setWidget(script_identifier);
	script_identifier_proxyWidget->setPos(boundingRect().topLeft() + QPointF(40, 30));

	reload_proxyWidget = new QGraphicsProxyWidget(this);
	reload = new GUI::Button();
	reload->setFixedSize(20, 20);
	reload_proxyWidget->setWidget(reload);
	reload_proxyWidget->setPos(boundingRect().topLeft() + QPointF(140, 30));

	QObject::connect(reload, &GUI::Button::clicked, [this]() {
		reloadDll();
	});

	if (script_id != "") {
		loadDLL(dynlib);

		FARPROC paintAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_renderGui").c_str());
		if (paintAddress) {
			paintGuiFunc = (void(*)(Script_Node*, QPainter*, const QStyleOptionGraphicsItem*, QWidget*))paintAddress;
		}
		FARPROC buildAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_buildGui").c_str());
		if (buildAddress) {
			buildGuiFunc = (void(*)(Script_Node*))buildAddress;
			buildGuiFunc(wrapper);
		}
		else {
			*LOG << ENDL << HTML_RED << "[DLL Binding]" << HTML_RESET << " Unable to resolve Script ID"; FLUSH;
		}
	}
}
GUI::NODE::EXEC::Script::~Script() {
	delete wrapper;
}

void GUI::NODE::EXEC::Script::clearIO() {
	for (auto it : inputs) {
		delete it;
	}
	inputs.clear();
	for (auto it : outputs) {
		delete it;
	}
	outputs.clear();
}

void GUI::NODE::EXEC::Script::addDataInput(const uint16& slot_id, const string& label, const CLASS::DATA::Type& type, const CLASS::DATA::Modifier& modifier) {
	PORT::Data_I_Port* value = new PORT::Data_I_Port(this, slot_id, QString::fromStdString(label), type, modifier);
	inputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
}

void GUI::NODE::EXEC::Script::addDataOutput(const uint16& slot_id, const string& label, const CLASS::DATA::Type& type, const CLASS::DATA::Modifier& modifier) {
	PORT::Data_O_Port* value = new PORT::Data_O_Port(this, slot_id, QString::fromStdString(label), type, modifier);
	outputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
}

void GUI::NODE::EXEC::Script::addExecInput(const uint16& slot_id, const string& label) {
	PORT::Exec_I_Port* value = new PORT::Exec_I_Port(this, slot_id, QString::fromStdString(label));
	inputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
}

void GUI::NODE::EXEC::Script::addExecOutput(const uint16& slot_id, const string& label) {
	PORT::Exec_O_Port* value = new PORT::Exec_O_Port(this, slot_id, QString::fromStdString(label));
	outputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
}

void GUI::NODE::EXEC::Script::renderDefault(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	Node::paint(painter, option, widget);
}

void GUI::NODE::EXEC::Script::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	if (paintGuiFunc)
		paintGuiFunc(wrapper, painter, option, widget);
	else
		Node::paint(painter, option, widget);
}

void GUI::NODE::EXEC::Script::reloadFunctions() {
	FARPROC paintAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_renderGui").c_str());
	if (paintAddress) {
		paintGuiFunc = (void(*)(Script_Node*, QPainter*, const QStyleOptionGraphicsItem*, QWidget*))paintAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_buildGui").c_str());
	if (buildAddress != nullptr) {
		buildGuiFunc = (void(*)(Script_Node*))buildAddress;
		buildGuiFunc(wrapper);
	}
	else {
		*LOG << ENDL << HTML_RED << "[DLL Binding]" << HTML_RESET << " Unable to resolve Script ID"; FLUSH;
	}
}

void GUI::NODE::EXEC::Script::reloadDll() {
	unloadDLL(dynlib);
	loadDLL(dynlib);
	reloadFunctions();
}

void GUI::NODE::EXEC::Script::recompile(const HINSTANCE& library) {
	dynlib = library;
	reloadFunctions();
}

GUI::NODE::EXEC::Script_Node::Script_Node(Script* node) :
	node(node)
{}
void GUI::NODE::EXEC::Script_Node::clearIO() const {
	node->clearIO();
}
void GUI::NODE::EXEC::Script_Node::addDataInput (const string& label, const CLASS::DATA::Type& type, const CLASS::DATA::Modifier& modifier) const {
	node->addDataInput (static_cast<uint16>(node->inputs .size()), label, type, modifier);
}
void GUI::NODE::EXEC::Script_Node::addDataOutput(const string& label, const CLASS::DATA::Type& type, const CLASS::DATA::Modifier& modifier) const {
	node->addDataOutput(static_cast<uint16>(node->outputs.size()), label, type, modifier);
}
void GUI::NODE::EXEC::Script_Node::addExecInput (const string& label) const {
	node->addExecInput (static_cast<uint16>(node->inputs .size()), label);
}
void GUI::NODE::EXEC::Script_Node::addExecOutput(const string& label) const {
	node->addExecOutput(static_cast<uint16>(node->outputs.size()), label);
}
void GUI::NODE::EXEC::Script_Node::renderDefault(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) const {
	node->renderDefault(painter, option, widget);
}
bool GUI::NODE::EXEC::Script_Node::isSelected() const {
	return node->isSelected();
}
QRectF GUI::NODE::EXEC::Script_Node::rect() const {
	return node->rect;
}
QString GUI::NODE::EXEC::Script_Node::label() const {
	return node->label;
}

GUI::NODE::EXEC::Tick::Tick(const ivec2& pos) {
	label = "Tick Update";
	type = CLASS::NODE::Type::EXEC;
	sub_type = e_to_u(CLASS::NODE::EXEC::Type::TICK);

	rect = QRectF(-100, -20, 200, 40);

	outputs.push_back(new PORT::Exec_O_Port(this, 0, "Tick"));
	outputs.push_back(new PORT::Data_O_Port(this, 1, "Delta", CLASS::DATA::Type::DOUBLE));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}

GUI::NODE::LINK::Pointer::Pointer(const ivec2& pos, const CLASS::DATA::Type& pointer_type) {
	label = "Pointer";
	type = CLASS::NODE::Type::LINK;
	sub_type = e_to_u(CLASS::NODE::LINK::Type::POINTER);

	this->pointer_type = pointer_type;
	pointer = nullptr;

	rect = QRectF(-100, -20, 200, 40);

	o_pointer = new PORT::Data_O_Port(this, 0, "Pointer", pointer_type);

	outputs.push_back(o_pointer);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}

void GUI::NODE::LINK::Pointer::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	Node::paint(painter, option, widget);
	if (pointer) {
		painter->setBrush(QColor(25, 25, 25));
		painter->drawRoundedRect(QRectF(rect.topLeft() + QPointF(4, 30), QSize(120, 20)), 5, 5);

		painter->setPen(Qt::white);
		switch (pointer_type) {
			case CLASS::DATA::Type::OBJECT: {
				painter->drawText(QRectF(rect.topLeft() + QPointF(14, 30), QSize(100, 20)), Qt::AlignLeft, QString::fromStdString(static_cast<CLASS::Object*>(pointer)->name));
				break;
			}
			case CLASS::DATA::Type::SCENE: {
				painter->drawText(QRectF(rect.topLeft() + QPointF(14, 30), QSize(100, 20)), Qt::AlignLeft, "Active Scene");
				break;
			}
		}
	}
}

GUI::NODE::LINK::Get::Get(const ivec2& pos) {
	label = "Get";
	type = CLASS::NODE::Type::LINK;
	sub_type = e_to_u(CLASS::NODE::LINK::Type::GET);
	mini_type = CLASS::NODE::LINK::GET::Type::NONE;

	i_pointer = nullptr;
	o_value   = nullptr;

	rect = QRectF(-100, -20, 200, 40);
	load_pos = QPointF(pos.x, pos.y);
}

GUI::NODE::LINK::GET::Field::Field(const ivec2& pos) :
	Get(pos)
{
	label = "Get Field";
	type = CLASS::NODE::Type::LINK;
	sub_type = e_to_u(CLASS::NODE::LINK::Type::GET);
	mini_type = CLASS::NODE::LINK::GET::Type::FIELD;

	QGraphicsProxyWidget* proxyWidget_id = new QGraphicsProxyWidget(this);
	field = new GUI::Value_Input();
	field->setFixedSize(180, 20);
	field->setPlaceholderText("Field");
	proxyWidget_id->setWidget(field);
	proxyWidget_id->setPos(boundingRect().topLeft() + QPointF(10, 50));

	i_pointer = new PORT::Data_I_Port(this, 0, "Pointer", CLASS::DATA::Type::ANY);

	o_value   = new PORT::Data_O_Port(this, 0, "Value", CLASS::DATA::Type::ANY);

	inputs.push_back(i_pointer);
	outputs.push_back(o_value);

	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
}

GUI::NODE::LINK::Set::Set(const ivec2& pos) {
	label = "Set";
	type = CLASS::NODE::Type::LINK;
	sub_type = e_to_u(CLASS::NODE::LINK::Type::SET);
	mini_type = CLASS::NODE::LINK::SET::Type::NONE;

	i_exec    = nullptr;
	i_pointer = nullptr;
	i_value   = nullptr;
	o_exec    = nullptr;
	o_value   = nullptr;

	rect = QRectF(-100, -20, 200, 40);
	load_pos = QPointF(pos.x, pos.y);
}

GUI::NODE::LINK::SET::Euler_Rotation_X::Euler_Rotation_X(const ivec2& pos) :
	Set(pos)
{
	label = "Set Euler Rotation X";
	mini_type = CLASS::NODE::LINK::SET::Type::EULER_ROTATION_X;

	i_exec    = new PORT::Exec_I_Port(this, 0, "Exec");
	i_pointer = new PORT::Data_I_Port(this, 1, "Pointer", CLASS::DATA::Type::OBJECT);
	i_value   = new PORT::Data_I_Port(this, 2, "Input Value", CLASS::DATA::Type::DOUBLE);

	o_exec    = new PORT::Exec_O_Port(this, 0, "Exec");
	o_value   = new PORT::Data_O_Port(this, 1, "Output Value", CLASS::DATA::Type::DOUBLE);

	inputs.push_back(i_exec);
	inputs.push_back(i_pointer);
	inputs.push_back(i_value);

	outputs.push_back(o_exec);
	outputs.push_back(o_value);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

GUI::NODE::MATH::MATH::MATH(const ivec2& pos) {
	type = CLASS::NODE::Type::MATH;
	sub_type = e_to_u(CLASS::NODE::MATH::Type::NONE);

	rect = QRectF(-100, -20, 200, 40);

	in_a  = new PORT::Data_I_Port(this, 0, "A", CLASS::DATA::Type::ANY);
	in_b  = new PORT::Data_I_Port(this, 1, "B", CLASS::DATA::Type::ANY);

	out_a = new PORT::Data_O_Port(this, 0, "Res", CLASS::DATA::Type::ANY);

	inputs.push_back(in_a);
	inputs.push_back(in_b);
	outputs.push_back(out_a);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}

GUI::NODE::MATH::Add::Add(const ivec2& pos) :
	MATH(pos)
{
	label = "Add";
	sub_type = e_to_u(CLASS::NODE::MATH::Type::ADD);
}

GUI::NODE::MATH::Sub::Sub(const ivec2& pos) :
	MATH(pos)
{
	label = "Subtract";
	sub_type = e_to_u(CLASS::NODE::MATH::Type::SUB);
}

GUI::NODE::MATH::Mul::Mul(const ivec2& pos) :
	MATH(pos)
{
	label = "Multiply";
	sub_type = e_to_u(CLASS::NODE::MATH::Type::MUL);
}

GUI::NODE::MATH::Div::Div(const ivec2& pos) :
	MATH(pos)
{
	label = "Divide";
	sub_type = e_to_u(CLASS::NODE::MATH::Type::DIV);
}

GUI::NODE::UTIL::Print::Print(const ivec2& pos) {
	label = "Print";
	type = CLASS::NODE::Type::UTIL;
	sub_type = e_to_u(CLASS::NODE::UTIL::Type::PRINT);

	rect = QRectF(-100, -20, 200, 40);

	i_exec  = new PORT::Exec_I_Port(this, 0, "Exec");
	i_value = new PORT::Data_I_Port(this, 1, "Value", CLASS::DATA::Type::ANY);

	o_exec  = new PORT::Exec_O_Port(this, 0, "Exec");

	inputs.push_back(i_exec);
	inputs.push_back(i_value);

	outputs.push_back(o_exec);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}

CLASS::Node_Tree* GUI::NODE::Node_Tree::toExecTree() {
	auto tree = new CLASS::Node_Tree();
	unordered_map<GUI::NODE::Node*, CLASS::Node*> node_map;

	for (GUI::NODE::Node* gui_node : this->nodes) {
		CLASS::Node* node = nullptr;
		switch (gui_node->type) {
			case CLASS::NODE::Type::EXEC: {
				switch (static_cast<CLASS::NODE::EXEC::Type>(gui_node->sub_type)) {
					case CLASS::NODE::EXEC::Type::SCRIPT: {
						auto t_node = new CLASS::NODE::EXEC::Script(dynamic_cast<GUI::NODE::EXEC::Script*>(gui_node)->script_identifier->text().toStdString());
						node = t_node;
						break;
					}
					case CLASS::NODE::EXEC::Type::COUNTER: {
						auto t_node = new CLASS::NODE::EXEC::Counter();
						node = t_node;
						break;
					}
					case CLASS::NODE::EXEC::Type::TICK: {
						auto t_node = new CLASS::NODE::EXEC::Tick();
						tree->tick = t_node;
						node = t_node;
						break;
					}
				}
				break;
			}
			case CLASS::NODE::Type::LINK: {
				switch (static_cast<CLASS::NODE::LINK::Type>(gui_node->sub_type)) {
					case CLASS::NODE::LINK::Type::POINTER: {
						auto t_node = new CLASS::NODE::LINK::Pointer();
						t_node->pointer_type = dynamic_cast<GUI::NODE::LINK::Pointer*>(gui_node)->pointer_type;
						t_node->pointer = dynamic_cast<GUI::NODE::LINK::Pointer*>(gui_node)->pointer;
						node = t_node;
						break;
					}
					case CLASS::NODE::LINK::Type::GET: {
						switch (static_cast<GUI::NODE::LINK::Get*>(gui_node)->mini_type) {
						case CLASS::NODE::LINK::GET::Type::FIELD: {
							auto t_node = new CLASS::NODE::LINK::GET::Field();
							t_node->field = dynamic_cast<GUI::NODE::LINK::GET::Field*>(gui_node)->field->text().toStdString();
							node = t_node;
							break;
						}
						}
						break;
					}
					case CLASS::NODE::LINK::Type::SET: {
						switch (static_cast<GUI::NODE::LINK::Set*>(gui_node)->mini_type) {
							case CLASS::NODE::LINK::SET::Type::EULER_ROTATION_X: {
								auto t_node = new CLASS::NODE::LINK::SET::Euler_Rotation_X();
								node = t_node;
								break;
							}
						}
						break;
					}
				}
				break;
			}
			case CLASS::NODE::Type::MATH: {
				switch (static_cast<CLASS::NODE::MATH::Type>(gui_node->sub_type)) {
					case CLASS::NODE::MATH::Type::ADD: {
						auto t_node = new CLASS::NODE::MATH::Add();
						node = t_node;
						break;
					}
					case CLASS::NODE::MATH::Type::SUB: {
						auto t_node = new CLASS::NODE::MATH::Sub();
						node = t_node;
						break;
					}
					case CLASS::NODE::MATH::Type::MUL: {
						auto t_node = new CLASS::NODE::MATH::Mul();
						node = t_node;
						break;
					}
					case CLASS::NODE::MATH::Type::DIV: {
						auto t_node = new CLASS::NODE::MATH::Div();
						node = t_node;
						break;
					}
				}
				break;
			}
			case CLASS::NODE::Type::UTIL: {
				switch (static_cast<CLASS::NODE::UTIL::Type>(gui_node->sub_type)) {
					case CLASS::NODE::UTIL::Type::PRINT: {
						auto t_node = new CLASS::NODE::UTIL::Print();
						node = t_node;
						break;
					}
				}
				break;
			}
		}
		tree->nodes.push_back(node);
		node_map[gui_node] = node;
		FILE->node_map[node] = gui_node;
	}
	//Connections
	for (GUI::NODE::Node* gui_node : this->nodes) {
		for (GUI::NODE::Port* port : gui_node->inputs) {
			if (port->type == CLASS::NODE::PORT::Type::DATA_I) { // TODO nullptr? Crash
				auto cast_port = static_cast<GUI::NODE::PORT::Data_I_Port*>(port);
				if (cast_port->connection) {
					for (auto port_r : node_map[gui_node]->inputs) {
						if (port_r->slot_id == port->slot_id) {
							for (auto port_l : node_map[cast_port->connection->port_l->node]->outputs) {
								if (port_l->slot_id == cast_port->connection->port_l->slot_id) {
									static_cast<CLASS::NODE::PORT::Data_I_Port*>(port_r)->connection = static_cast<CLASS::NODE::PORT::Data_O_Port*>(port_l);
									#ifdef LOG1
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
			if (port->type == CLASS::NODE::PORT::Type::EXEC_O) {
				auto cast_port = static_cast<GUI::NODE::PORT::Exec_O_Port*>(port);
				if (cast_port->connection) {
					for (auto port_l : node_map[gui_node]->outputs) {
						if (port_l->slot_id == port->slot_id) {
							for (auto port_r : node_map[cast_port->connection->port_r->node]->inputs) {
								if (port_r->slot_id == cast_port->connection->port_r->slot_id) {
									static_cast<CLASS::NODE::PORT::Exec_O_Port*>(port_l)->connection = static_cast<CLASS::NODE::PORT::Exec_I_Port*>(port_r);
									#ifdef LOG1
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
	return tree;
}