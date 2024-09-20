#include "Object/Nodes/Math.hpp"

KL::NODE::MATH::Arithmetic::Arithmetic() {
	type = NODE::Type::MATH;
	sub_type = e_to_us(NODE::MATH::Type::ARITHMETIC);
	mini_type = ARITHMETIC::Type::NONE;

	i_a   = new PORT::Data_I_Port(this, 0, PROP::Type::ANY);
	i_b   = new PORT::Data_I_Port(this, 1, PROP::Type::ANY);

	o_res = new PORT::Data_O_Port(this, 0, PROP::Type::ANY);

	inputs.push_back(i_a);
	inputs.push_back(i_b);
	outputs.push_back(o_res);
}

KL::NODE::MATH::ARITHMETIC::Multiplication::Multiplication() {
	mini_type = ARITHMETIC::Type::MULTIPLICATION;
}

KL::Prop KL::NODE::MATH::ARITHMETIC::Multiplication::getData(const uint16& slot_id) const {
	return i_a->getData() * i_b->getData();
}

KL::NODE::MATH::ARITHMETIC::Addition::Addition() {
	mini_type = ARITHMETIC::Type::ADDITION;
}

KL::Prop KL::NODE::MATH::ARITHMETIC::Addition::getData(const uint16& slot_id) const {
	return i_a->getData() + i_b->getData();
}

KL::NODE::MATH::ARITHMETIC::Subtraction::Subtraction() {
	mini_type = ARITHMETIC::Type::SUBTRACTION;
}

KL::Prop KL::NODE::MATH::ARITHMETIC::Subtraction::getData(const uint16& slot_id) const {
	return i_a->getData() - i_b->getData();
}

KL::NODE::MATH::ARITHMETIC::Division::Division() {
	mini_type = ARITHMETIC::Type::DIVISION;
}

KL::Prop KL::NODE::MATH::ARITHMETIC::Division::getData(const uint16& slot_id) const {
	return i_a->getData() / i_b->getData();
}

KL::NODE::MATH::ARITHMETIC::Power::Power() {
	mini_type = ARITHMETIC::Type::POWER;
}

KL::Prop KL::NODE::MATH::ARITHMETIC::Power::getData(const uint16& slot_id) const {
	return i_a->getData().pow(i_b->getData());
}