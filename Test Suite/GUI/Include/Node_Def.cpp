#include "Node_Def.hpp"

#include "Compiler.hpp"

MATH_Node::MATH_Node(const QPoint& pos) : Node(pos) {
	build();
	updateRect();
}

void MATH_Node::build() {
	Data_I_Port* add_a = new Data_I_Port(this, 0, "In A","A");
	add_a->default_value = 1.5;
	Data_I_Port* add_b = new Data_I_Port(this, 1, "In B", "B");
	add_b->default_value = 2.5;
	inputs.push_back(add_a);
	inputs.push_back(add_b);

	Data_O_Port* result = new Data_O_Port(this, 0, "Out", "Result");
	outputs.push_back(result);
}

void MATH_Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	Node::paint(painter, option, widget);

	painter->setBrush(QColor(25, 25, 25));
	if (static_cast<Data_I_Port*>(inputs[0])->connection and static_cast<Data_I_Port*>(inputs[0])->connection->port_l) {
		const QString number_a = QString::number(any_cast<double>(inputs[0]->getData()));
		painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(50, 31), QSize(80, 18)), 5, 5);
		painter->drawText(QRectF(boundingRect().topLeft() + QPointF(60, 31), QSize(60, 18)), Qt::AlignLeft, number_a);
	}
	else {
		const QString number_a = QString::number(any_cast<double>(inputs[0]->getData()));
		painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(-80, 36), QSize(80, 18)), 5, 5);
		painter->drawText(QRectF(boundingRect().topLeft() + QPointF(-70, 36), QSize(60, 18)), Qt::AlignRight, number_a);
	}
	if (static_cast<Data_I_Port*>(inputs[1])->connection and static_cast<Data_I_Port*>(inputs[1])->connection->port_l) {
		const QString number_b = QString::number(any_cast<double>(inputs[1]->getData()));
		painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(50, 51), QSize(80, 18)), 5, 5);
		painter->drawText(QRectF(boundingRect().topLeft() + QPointF(60, 51), QSize(60, 18)), Qt::AlignLeft, number_b);
	}
	else {
		const QString number_b = QString::number(any_cast<double>(inputs[1]->getData()));
		painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(-80, 56), QSize(80, 18)), 5, 5);
		painter->drawText(QRectF(boundingRect().topLeft() + QPointF(-70, 56), QSize(60, 18)), Qt::AlignRight, number_b);
	}
}

VIEW_Node::VIEW_Node(const QPoint& pos) : Node(pos) {
	label = "Viewer";
	build();
	updateRect();
}

void VIEW_Node::build() {
	Exec_I_Port* in = new Exec_I_Port(this, 0, "", "In");
	inputs.push_back(in);

	Exec_O_Port* out = new Exec_O_Port(this, 0, "", "0ut");
	outputs.push_back(out);

	Data_I_Port* value = new Data_I_Port(this, 1, "", "Value");
	value->default_value = 0.5;
	inputs.push_back(value);
}

void VIEW_Node::exec(const string& slot_id) {
	update();
	outputs[0]->exec();
}

any VIEW_Node::getData(const string& slot_id) const {
	return any_cast<double>(inputs[1]->getData());
}

void VIEW_Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	Node::paint(painter, option, widget);
	painter->setBrush(QColor(25, 25, 25));
	if (static_cast<Data_I_Port*>(inputs[1])->connection and static_cast<Data_I_Port*>(inputs[1])->connection->port_l) {
		QString number = QString::number(any_cast<double>(getData("")));
		painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(70, 31), QSize(80, 18)), 5, 5);
		painter->drawText(QRectF(boundingRect().topLeft() + QPointF(100, 31), QSize(60, 18)), Qt::AlignLeft, number);
	}
	else {
		QString number = QString::number(any_cast<double>(getData("")));
		painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(-80, 56), QSize(80, 18)), 5, 5);
		painter->drawText(QRectF(boundingRect().topLeft() + QPointF(-70, 56), QSize(60, 18)), Qt::AlignRight, number);
	}
}

REROUTE_Node::REROUTE_Node(const QPoint& pos) {
	label = "Reroute";
	build();
	rect = QRectF(-10, -10, 20, 20);
}

void REROUTE_Node::build() {
	Data_I_Port* in = new Data_I_Port(this, 0, "", "");
	in->default_value = 0.5;
	inputs.push_back(in);
	Data_O_Port* out = new Data_O_Port(this, 0, "", "");
	outputs.push_back(out);
}

any REROUTE_Node::getData(const string& slot_id) const {
	return any_cast<double>(inputs[0]->getData());
}

void REROUTE_Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(QColor(40, 40, 40));
	painter->drawRoundedRect(boundingRect(), 5, 5);

	painter->setBrush(inputs[0]->color);
	painter->drawEllipse(boundingRect().marginsRemoved(QMarginsF(2.5, 2.5, 2.5, 2.5)));
	painter->drawLine(boundingRect().topLeft() + QPointF(10, 2.5), boundingRect().bottomLeft() + QPointF(10, -2.5));
}

EXEC_Node::EXEC_Node(const QPoint& pos) : Node(pos) {
	label = "EXEC";
	build();
	timer.setInterval(50);
	QObject::connect(&timer, &QTimer::timeout, [this]() {exec("Passthrough"); });
	timer.start();
	updateRect();
}

void EXEC_Node::build() {
	Exec_O_Port* value = new Exec_O_Port(this, 0, "Passthrough", "Tick");
	outputs.push_back(value);
}

void EXEC_Node::exec(const string& slot_id) {
	outputs[0]->exec();
}

TICK_COUNTER_Node::TICK_COUNTER_Node(const QPoint& pos) : Node(pos) {
	label = "COUNT";
	count = 0;
	build();
	updateRect();
}

void TICK_COUNTER_Node::build() {
	Exec_I_Port* value = new Exec_I_Port(this, 0, "In", "Tick");
	inputs.push_back(value);
}

void TICK_COUNTER_Node::exec(const string& slot_id) {
	count++;
	update();
}

void TICK_COUNTER_Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	Node::paint(painter, option, widget);
	painter->setBrush(QColor(25, 25, 25));
	QString number = QString::number(count);
	painter->drawRoundedRect(QRectF(boundingRect().topLeft() + QPointF(90, 31), QSize(80, 18)), 5, 5);
	painter->drawText(QRectF(boundingRect().topLeft() + QPointF(100, 31), QSize(60, 18)), Qt::AlignLeft, number);
}

SEQUENCE_Node::SEQUENCE_Node(const QPoint& pos) : Node(pos) {
	label = "SEQUENCE";
	count = 1;
	build();
	updateRect();
}

void SEQUENCE_Node::build() {
	Exec_I_Port* in = new Exec_I_Port(this, 0,"", "In");
	inputs.push_back(in);

	Exec_O_Port* out = new Exec_O_Port(this, 0,"", "0ut");
	outputs.push_back(out);

	QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget(this);
	add = new GUI::Button();
	add->setFixedSize(20, 20);
	proxyWidget->setWidget(add);

	proxyWidget->setPos(boundingRect().topLeft() + QPointF(70, 30));

	QObject::connect(add, &GUI::Button::clicked, [this]() {
		Exec_O_Port* value = new Exec_O_Port(this, count, "",  QString::number(count));
		outputs.push_back(value);
		count++;
		updateRect();
	});
}

void SEQUENCE_Node::exec(const string& slot_id) {
	for (Port* output : outputs) {
		output->exec();
	}
}

SCRIPT_Node::SCRIPT_Node(const QPoint& pos) {
	label = "SCRIPT";
	script_id = "Script_ID";
	count_in = 1;
	count_out = 1;
	build();
	updateRect();
}

void SCRIPT_Node::build() {
	in = new Exec_I_Port(this, 0, "Passthrough", "I");
	exec_inputs["Passthrough"] = in;
	inputs.push_back(in);

	out = new Exec_O_Port(this, 0, "Passthrough", "0");
	exec_outputs["Passthrough"] = out;
	outputs.push_back(out);

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
		script_id = script_identifier->text().toStdString();
		reloadFunctions();
	});

	loadDLL(dynlib, "DLL.dll");

	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(SCRIPT_Node*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (any(*)(const SCRIPT_Node*, const string&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(SCRIPT_Node*))buildAddress;
		buildFunc(this);
	}
}

void SCRIPT_Node::addDataInput(const string& slot_id, const string& label) {
	Data_I_Port* value = new Data_I_Port(this, count_in, slot_id, QString::fromStdString(label));
	data_inputs[slot_id] = value;
	inputs.push_back(value);
	count_in++;
	updateRect();
};

void SCRIPT_Node::addDataOutput(const string& slot_id, const string& label) {
	Data_O_Port* value = new Data_O_Port(this, count_out, slot_id, QString::fromStdString(label));
	data_outputs[slot_id] = value;
	outputs.push_back(value);
	count_out++;
	updateRect();
};

void SCRIPT_Node::addExecInput(const string& slot_id, const string& label) {
	Exec_I_Port* value = new Exec_I_Port(this, count_in, slot_id, QString::fromStdString(label));
	exec_inputs[slot_id] = value;
	inputs.push_back(value);
	count_in++;
	updateRect();
};

void SCRIPT_Node::addExecOutput(const string& slot_id, const string& label) {
	Exec_O_Port* value = new Exec_O_Port(this, count_out, slot_id, QString::fromStdString(label));
	exec_outputs[slot_id] = value;
	outputs.push_back(value);
	count_out++;
	updateRect();
};

void SCRIPT_Node::reloadFunctions() {
	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(SCRIPT_Node*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (any(*)(const SCRIPT_Node*, const string&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(SCRIPT_Node*))buildAddress;
		buildFunc(this);
	}
}

void SCRIPT_Node::reloadDll() {
	unloadDLL(dynlib);
	loadDLL(dynlib, "DLL.dll");

	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(SCRIPT_Node*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (any(*)(const SCRIPT_Node*, const string&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(SCRIPT_Node*))buildAddress;
		buildFunc(this);
	}
}

void SCRIPT_Node::clearIO() {
	for (auto it = inputs.begin() + 1; it != inputs.end(); ++it) { delete* it; }
	inputs.erase(inputs.begin() + 1, inputs.end());
	for (auto it = outputs.begin() + 1; it != outputs.end(); ++it) { delete* it; }
	outputs.erase(outputs.begin() + 1, outputs.end());

	data_inputs.clear();
	data_outputs.clear();
	exec_inputs.clear();
	exec_outputs.clear();

	in = new Exec_I_Port(this, 0, "Passthrough", "I");
	exec_inputs["Passthrough"] = in;
	inputs.push_back(in);

	out = new Exec_O_Port(this, 0, "Passthrough", "0");
	exec_outputs["Passthrough"] = out;
	outputs.push_back(out);

	count_in = 1;
	count_out = 1;
}

void SCRIPT_Node::exec(const string& slot_id) {
	exec_outputs["Passthrough"]->exec();
	if (execFunc) execFunc(this);
}

any SCRIPT_Node::getData(const string& slot_id) const {
	if (getDataFunc) return getDataFunc(this, slot_id);
	return 0.0;
}

any SCRIPT_Node::getInputData(const string& slot_id) const {
	auto it = data_inputs.find("val");
	if (it != data_inputs.end())
		return data_inputs.at(slot_id)->getData();
	return MIN_DVEC1;
}

void SCRIPT_Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	for (const auto& [key, port] : data_inputs) {
		painter->setBrush(QColor(25, 25, 25));
		if (port->connection and port->connection->port_l) { // Connected
			Node::paint(painter, option, widget);
			const QString number_b = QString::number(any_cast<double>(port->getData()));
			painter->drawRoundedRect(QRectF(port->boundingRect().topLeft() + QPointF(80, 1), QSize(80, 18)), 5, 5);
			painter->drawText(QRectF(port->boundingRect().topLeft() + QPointF(90, 1), QSize(60, 18)), Qt::AlignLeft, number_b);
		}
		else { // Not Connected
			const QString number_b = QString::number(any_cast<double>(port->getData()));
			painter->drawRoundedRect(QRectF(port->boundingRect().topLeft() + QPointF(-70, 6), QSize(80, 18)), 5, 5);
			painter->drawText(QRectF(port->boundingRect().topLeft() + QPointF(-70, 6), QSize(60, 18)), Qt::AlignRight, number_b);
			Node::paint(painter, option, widget);
		}
	}
}