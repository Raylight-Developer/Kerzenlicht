#include "Node_GUI_Def.hpp"

GUI::NODE::EXEC::Tick::Tick(const ivec2& pos) {
	label = "Tick";
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::TICK);

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	outputs.push_back(new PORT::Exec_O_Port(this, 0, "Tick" , "0"));
	outputs.push_back(new PORT::Data_O_Port(this, 1, "Delta", "1", CLASS::NODE::DATA::Type::DOUBLE));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

GUI::NODE::MATH::MATH::MATH(const ivec2& pos) {
	type = CLASS::NODE::Type::MATH;

	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(QPointF(pos.x, pos.y));

	inputs. push_back(in_a  = new PORT::Data_I_Port(this, 0, "A", "0", CLASS::NODE::DATA::Type::ANY));
	inputs. push_back(in_b  = new PORT::Data_I_Port(this, 1, "B", "1", CLASS::NODE::DATA::Type::ANY));
	outputs.push_back(out_a = new PORT::Data_O_Port(this, 0, "R", "0", CLASS::NODE::DATA::Type::ANY));

	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

void GUI::NODE::MATH::MATH::onPortConnect(Port* port, Connection* connection) {
	if      (port == in_a) {
		//static_cast<PORT::Data_I_Port*>(connection->port_l)->any_data_type;
		in_b->color = port->color;
		out_a->color = port->color;
	}
	else if (port == in_b) {
		in_a->color = port->color;
		out_a->color = port->color;
	}
	else if (port == out_a) {
		in_a->color = port->color;
		in_b->color = port->color;
	}
	connection->color = port->color;
}

void GUI::NODE::MATH::MATH::onPortDisconnect(Port* port) {
	if (!in_a->connection and !in_b->connection and out_a->outgoing_connections.empty()) {
		in_a->color  = typeColor(in_a->data_type);
		in_b->color  = typeColor(in_b->data_type);
		out_a->color = typeColor(out_a->data_type);
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
	label = "Sub";
	sub_type = ETOU(CLASS::NODE::MATH::Type::SUB);
}

GUI::NODE::MATH::Mul::Mul(const ivec2& pos) :
	MATH(pos)
{
	label = "Mul";
	sub_type = ETOU(CLASS::NODE::MATH::Type::MUL);
}

GUI::NODE::MATH::Div::Div(const ivec2& pos) :
	MATH(pos)
{
	label = "Div";
	sub_type = ETOU(CLASS::NODE::MATH::Type::DIV);
}