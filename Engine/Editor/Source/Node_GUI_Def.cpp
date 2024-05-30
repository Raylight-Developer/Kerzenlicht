#include "Node_GUI_Def.hpp"

GUI::NODE::EXEC::Tick::Tick(const ivec2& pos) {
	label = "Tick Update";
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::TICK);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	outputs.push_back(new PORT::Exec_O_Port(this, 0, "Tick" , "0"));
	outputs.push_back(new PORT::Data_O_Port(this, 1, "Delta", "1", CLASS::NODE::DATA::Type::DOUBLE));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

GUI::NODE::LINK::Pointer::Pointer(const ivec2& pos) {
	label = "Pointer";
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::POINTER);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	outputs.push_back(new PORT::Data_O_Port(this, 1, "Pointer", "0", CLASS::NODE::DATA::Type::ANY));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

GUI::NODE::MATH::MATH::MATH(const ivec2& pos) {
	type = CLASS::NODE::Type::MATH;

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	inputs. push_back(in_a  = new PORT::Data_I_Port(this, 0, "A", "0", CLASS::NODE::DATA::Type::ANY));
	inputs. push_back(in_b  = new PORT::Data_I_Port(this, 1, "B", "1", CLASS::NODE::DATA::Type::ANY));
	outputs.push_back(out_a = new PORT::Data_O_Port(this, 0, "R", "0", CLASS::NODE::DATA::Type::ANY));

	data_type = CLASS::NODE::DATA::Type::ANY;

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

void GUI::NODE::MATH::MATH::onDataTypeSet(const CLASS::NODE::DATA::Type& type) {
	if (data_type == CLASS::NODE::DATA::Type::ANY) {
		cout << "Data Type Set For " << label.toStdString() << endl;
		data_type = type;

		const QColor col = typeColor(type);
		in_a->color  = col;
		in_b->color  = col;
		out_a->color = col;

		if (in_a->connection) in_a->connection->port_l->node->onDataTypeSet(type);
		if (in_b->connection) in_b->connection->port_l->node->onDataTypeSet(type);
		for (Connection* conn: out_a->outgoing_connections)
			conn->port_r->node->onDataTypeSet(type);
	}
}

void GUI::NODE::MATH::MATH::onDataTypeUnset() {
	if (data_type != CLASS::NODE::DATA::Type::ANY) {
		data_type = CLASS::NODE::DATA::Type::ANY;

		const QColor col = typeColor(data_type);
		in_a->color = col;
		in_b->color = col;
		out_a->color = col;

		if (in_a->connection) in_a->connection->port_l->node->onDataTypeUnset();
		if (in_b->connection) in_b->connection->port_l->node->onDataTypeUnset();
		for (Connection* conn : out_a->outgoing_connections)
			conn->port_r->node->onDataTypeUnset();
	}
}

void GUI::NODE::MATH::MATH::onPortConnect(Port* port, Connection* connection) {
	if (data_type == CLASS::NODE::DATA::Type::ANY) {
		if (connection->data_type != CLASS::NODE::DATA::Type::ANY){
			onDataTypeSet(connection->data_type);
		}
	}
	else if (connection->data_type == CLASS::NODE::DATA::Type::ANY) {
		connection->data_type = data_type;
		connection->color = typeColor(data_type);
	}
}

void GUI::NODE::MATH::MATH::onPortDisconnect(Port* port) {
	if (!in_a->connection and !in_b->connection and out_a->outgoing_connections.empty()) {
		onDataTypeUnset();
		// TODO cascade Unset if there are no hard-defined types in graph
	}
}

GUI::NODE::MATH::Add::Add(const ivec2& pos) :
	MATH(pos)
{
	label = "Add";
	sub_type = ETOU(CLASS::NODE::MATH::Type::ADD);
}

GUI::NODE::MATH::Sub::Sub(const ivec2& pos) :
	MATH(pos)
{
	label = "Subtract";
	sub_type = ETOU(CLASS::NODE::MATH::Type::SUB);
}

GUI::NODE::MATH::Mul::Mul(const ivec2& pos) :
	MATH(pos)
{
	label = "Multiply";
	sub_type = ETOU(CLASS::NODE::MATH::Type::MUL);
}

GUI::NODE::MATH::Div::Div(const ivec2& pos) :
	MATH(pos)
{
	label = "Divide";
	sub_type = ETOU(CLASS::NODE::MATH::Type::DIV);
}