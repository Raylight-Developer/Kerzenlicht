#include "Node_GUI_Def.hpp"

#include "Object/Object.hpp"

using namespace GUI::NODE;

EXEC::Counter::Counter(const ivec2& pos) {
	label = "Tick Counter";
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::COUNTER);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	i_exec  = new PORT::Exec_I_Port(this, 0, "Tick");

	o_exec  = new PORT::Exec_O_Port(this, 0, "Tick");
	o_count = new PORT::Data_O_Port(this, 1, "Count", CLASS::NODE::DATA::Type::UINT);

	inputs.push_back(i_exec);
	outputs.push_back(o_exec);
	outputs.push_back(o_count);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

GUI::NODE::EXEC::Script::Script(const ivec2& pos) {
	label = "Script";
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::COUNTER);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	QGraphicsProxyWidget* proxyWidget_id = new QGraphicsProxyWidget(this);
	script_identifier = new GUI::Value_Input();
	script_identifier->setFixedSize(90, 20);
	script_identifier->setPlaceholderText("Script_ID");
	script_identifier->setText("Script_ID");
	proxyWidget_id->setWidget(script_identifier);
	proxyWidget_id->setPos(boundingRect().topLeft() + QPointF(40, 30));

	QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget(this);
	reload = new GUI::Button();
	reload->setFixedSize(20, 20);
	proxyWidget->setWidget(reload);
	proxyWidget->setPos(boundingRect().topLeft() + QPointF(140, 30));

	QObject::connect(reload, &GUI::Button::clicked, [this]() {
		reloadFunctions();
	});

	loadDLL(dynlib, "D:/Kerzenlicht Renderer/x64/Debug/Runtime.dll");

	FARPROC buildAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_buildGui").c_str());
	if (buildAddress != NULL) {
		buildGuiFunc = (void(*)(Script*))buildAddress;
		buildGuiFunc(this);
	}
}

void EXEC::Script::clearIO() {
	for (auto it = inputs.begin(); it != inputs.end(); ++it) { delete* it; }
	inputs.erase(inputs.begin(), inputs.end());
	for (auto it = outputs.begin(); it != outputs.end(); ++it) { delete* it; }
	outputs.erase(outputs.begin(), outputs.end());
}

void EXEC::Script::addDataInput(const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) {
	PORT::Data_I_Port* value = new PORT::Data_I_Port(this, slot_id, QString::fromStdString(label), type, modifier);
	inputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void EXEC::Script::addDataOutput(const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) {
	PORT::Data_O_Port* value = new PORT::Data_O_Port(this, slot_id, QString::fromStdString(label), type, modifier);
	outputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void EXEC::Script::addExecInput(const uint16& slot_id, const string& label) {
	PORT::Exec_I_Port* value = new PORT::Exec_I_Port(this, slot_id, QString::fromStdString(label));
	inputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void EXEC::Script::addExecOutput(const uint16& slot_id, const string& label) {
	PORT::Exec_O_Port* value = new PORT::Exec_O_Port(this, slot_id, QString::fromStdString(label));
	outputs.push_back(value);
	value->rect.moveTopLeft(value->rect.topLeft() + QPointF(0, 20));
	rect.setHeight(60 + max(inputs.size(), outputs.size()) * 20);
};

void EXEC::Script::reloadFunctions() {
	FARPROC buildAddress = GetProcAddress(dynlib, (script_identifier->text().toStdString() + "_buildGui").c_str());
	if (buildAddress != NULL) {
		buildGuiFunc = (void(*)(Script*))buildAddress;
		buildGuiFunc(this);
	}
}

void EXEC::Script::reloadDll() {
	unloadDLL(dynlib);
	loadDLL(dynlib, "D:/Kerzenlicht Renderer/x64/Debug/Runtime.dll");
	reloadFunctions();
}

EXEC::Tick::Tick(const ivec2& pos) {
	label = "Tick Update";
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::TICK);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	outputs.push_back(new PORT::Exec_O_Port(this, 0, "Tick"));
	outputs.push_back(new PORT::Data_O_Port(this, 1, "Delta", CLASS::NODE::DATA::Type::DOUBLE));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

LINK::Pointer::Pointer(const ivec2& pos) {
	label = "Pointer";
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::POINTER);

	pointer_type = CLASS::NODE::DATA::Type::NONE;
	pointer = nullptr;

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	out_pointer = new PORT::Data_O_Port(this, 0, "Pointer", CLASS::NODE::DATA::Type::OBJECT);

	outputs.push_back(out_pointer);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

void LINK::Pointer::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	Node::paint(painter, option, widget);
	if (pointer) {
		painter->setBrush(QColor(25, 25, 25));
		painter->drawRoundedRect(QRectF(rect.topLeft() + QPointF(4, 30), QSize(120, 20)), 5, 5);

		painter->setPen(Qt::white);
		if (pointer_type == CLASS::NODE::DATA::Type::OBJECT)
			painter->drawText(QRectF(rect.topLeft() + QPointF(14, 30), QSize(100, 20)), Qt::AlignLeft, QString::fromStdString(static_cast<CLASS::Object*>(pointer)->name));
	}
}

LINK::Get::Get(const ivec2& pos) {
	label = "Pointer";
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::POINTER);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	outputs.push_back(new PORT::Data_O_Port(this, 0, "Pointer", CLASS::NODE::DATA::Type::ANY));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

LINK::Set::Set(const ivec2& pos) {
	label = "Set";
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::SET);
	micro_type = CLASS::NODE::LINK::SET::Type::NONE;

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	i_exec    = new PORT::Exec_I_Port(this, 0, "Exec");
	i_pointer = new PORT::Data_I_Port(this, 1, "Pointer", CLASS::NODE::DATA::Type::ANY);
	i_value   = new PORT::Data_I_Port(this, 2, "Input Value", CLASS::NODE::DATA::Type::ANY);

	o_exec    = new PORT::Exec_O_Port(this, 0, "Exec");
	o_value   = new PORT::Data_O_Port(this, 1, "Output Value", CLASS::NODE::DATA::Type::ANY);

	inputs.push_back(i_exec);
	inputs.push_back(i_pointer);
	inputs.push_back(i_value);
	outputs.push_back(o_exec);
	outputs.push_back(o_value);

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

LINK::SET::Euler_Rotation_X::Euler_Rotation_X(const ivec2& pos) :
	Set(pos)
{
	label = "Set Euler Rotation X";
	micro_type = CLASS::NODE::LINK::SET::Type::EULER_ROTATION_X;

	i_pointer->setDataType(CLASS::NODE::DATA::Type::OBJECT);
	i_value->setDataType(CLASS::NODE::DATA::Type::DOUBLE);
	o_value->setDataType(CLASS::NODE::DATA::Type::DOUBLE);
}

MATH::MATH::MATH(const ivec2& pos) {
	type = CLASS::NODE::Type::MATH;

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	in_a  = new PORT::Data_I_Port(this, 0, "A", CLASS::NODE::DATA::Type::ANY);
	in_b  = new PORT::Data_I_Port(this, 1, "B", CLASS::NODE::DATA::Type::ANY);

	out_a = new PORT::Data_O_Port(this, 0, "Res", CLASS::NODE::DATA::Type::ANY);

	inputs.push_back(in_a);
	inputs.push_back(in_b);
	outputs.push_back(out_a);

	data_type = CLASS::NODE::DATA::Type::ANY;

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

MATH::Add::Add(const ivec2& pos) :
	MATH(pos)
{
	label = "Add";
	sub_type = ETOU(CLASS::NODE::MATH::Type::ADD);
}

MATH::Sub::Sub(const ivec2& pos) :
	MATH(pos)
{
	label = "Subtract";
	sub_type = ETOU(CLASS::NODE::MATH::Type::SUB);
}

MATH::Mul::Mul(const ivec2& pos) :
	MATH(pos)
{
	label = "Multiply";
	sub_type = ETOU(CLASS::NODE::MATH::Type::MUL);
}

MATH::Div::Div(const ivec2& pos) :
	MATH(pos)
{
	label = "Divide";
	sub_type = ETOU(CLASS::NODE::MATH::Type::DIV);
}