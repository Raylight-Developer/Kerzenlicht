#include "Node_GUI_Def.hpp"

#include "Object/Object.hpp"

GUI::NODE::EXEC::Counter::Counter(const ivec2& pos) {
	label = "Tick Counter";
	type = CLASS::NODE::Type::EXEC;
	sub_type = e_to_u(CLASS::NODE::EXEC::Type::COUNTER);

	rect = QRectF(-100, -20, 200, 40);

	i_exec  = new PORT::Exec_I_Port(this, 0, "Tick");

	o_exec  = new PORT::Exec_O_Port(this, 0, "Tick");
	o_count = new PORT::Data_O_Port(this, 1, "Count", CLASS::NODE::DATA::Type::UINT);

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

		FARPROC buildAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_buildGui").c_str());
		if (buildAddress != nullptr) {
			buildGuiFunc = (void(*)(Script_Node*))buildAddress;
			buildGuiFunc(wrapper);
		}
		else {
			*LOG << ENDL << HTML_RED << "[DLL Binding]" << HTML_RESET << " Unable to resolve Script ID"; FLUSH
		}
	}
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

void GUI::NODE::EXEC::Script::addDataInput(const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) {
	PORT::Data_I_Port* value = new PORT::Data_I_Port(this, slot_id, QString::fromStdString(label), type, modifier);
	inputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void GUI::NODE::EXEC::Script::addDataOutput(const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) {
	PORT::Data_O_Port* value = new PORT::Data_O_Port(this, slot_id, QString::fromStdString(label), type, modifier);
	outputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void GUI::NODE::EXEC::Script::addExecInput(const uint16& slot_id, const string& label) {
	PORT::Exec_I_Port* value = new PORT::Exec_I_Port(this, slot_id, QString::fromStdString(label));
	inputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void GUI::NODE::EXEC::Script::addExecOutput(const uint16& slot_id, const string& label) {
	PORT::Exec_O_Port* value = new PORT::Exec_O_Port(this, slot_id, QString::fromStdString(label));
	outputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void GUI::NODE::EXEC::Script::reloadFunctions() {
	FARPROC buildAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_buildGui").c_str());
	if (buildAddress != nullptr) {
		buildGuiFunc = (void(*)(Script_Node*))buildAddress;
		buildGuiFunc(wrapper);
	}
	else {
		*LOG << ENDL << HTML_RED << "[DLL Binding]" << HTML_RESET << " Unable to resolve Script ID"; FLUSH
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
void GUI::NODE::EXEC::Script_Node::addDataInput (const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) const {
	node->addDataInput (slot_id, label, type, modifier);
}
void GUI::NODE::EXEC::Script_Node::addDataOutput(const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) const {
	node->addDataOutput(slot_id, label, type, modifier);
}
void GUI::NODE::EXEC::Script_Node::addExecInput (const uint16& slot_id, const string& label) const {
	node->addExecInput (slot_id, label);
}
void GUI::NODE::EXEC::Script_Node::addExecOutput(const uint16& slot_id, const string& label) const {
	node->addExecOutput(slot_id, label);
}

GUI::NODE::EXEC::Tick::Tick(const ivec2& pos) {
	label = "Tick Update";
	type = CLASS::NODE::Type::EXEC;
	sub_type = e_to_u(CLASS::NODE::EXEC::Type::TICK);

	rect = QRectF(-100, -20, 200, 40);

	outputs.push_back(new PORT::Exec_O_Port(this, 0, "Tick"));
	outputs.push_back(new PORT::Data_O_Port(this, 1, "Delta", CLASS::NODE::DATA::Type::DOUBLE));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}

GUI::NODE::LINK::Pointer::Pointer(const ivec2& pos, const CLASS::NODE::DATA::Type& pointer_type) {
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
			case CLASS::NODE::DATA::Type::OBJECT: {
				painter->drawText(QRectF(rect.topLeft() + QPointF(14, 30), QSize(100, 20)), Qt::AlignLeft, QString::fromStdString(static_cast<CLASS::Object*>(pointer)->name));
				break;
			}
			case CLASS::NODE::DATA::Type::SCENE: {
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

	i_pointer = new PORT::Data_I_Port(this, 0, "Pointer", CLASS::NODE::DATA::Type::ANY);

	o_value   = new PORT::Data_O_Port(this, 0, "Value", CLASS::NODE::DATA::Type::ANY);

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
	i_pointer = new PORT::Data_I_Port(this, 1, "Pointer", CLASS::NODE::DATA::Type::OBJECT);
	i_value   = new PORT::Data_I_Port(this, 2, "Input Value", CLASS::NODE::DATA::Type::DOUBLE);

	o_exec    = new PORT::Exec_O_Port(this, 0, "Exec");
	o_value   = new PORT::Data_O_Port(this, 1, "Output Value", CLASS::NODE::DATA::Type::DOUBLE);

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

	in_a  = new PORT::Data_I_Port(this, 0, "A", CLASS::NODE::DATA::Type::ANY);
	in_b  = new PORT::Data_I_Port(this, 1, "B", CLASS::NODE::DATA::Type::ANY);

	out_a = new PORT::Data_O_Port(this, 0, "Res", CLASS::NODE::DATA::Type::ANY);

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
	i_value = new PORT::Data_I_Port(this, 1, "Value", CLASS::NODE::DATA::Type::ANY);

	o_exec  = new PORT::Exec_O_Port(this, 0, "Exec");

	inputs.push_back(i_exec);
	inputs.push_back(i_value);

	outputs.push_back(o_exec);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
	load_pos = QPointF(pos.x, pos.y);
}