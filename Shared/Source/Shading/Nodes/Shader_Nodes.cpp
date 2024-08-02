#include "Shading/Nodes/Shader_Nodes.hpp"

using namespace SHADER::NODE;

EXEC::Tick::Tick() {
	type = NODE::Type::EXEC;
	sub_type = e_to_u(Type::TICK);

	delta = new dvec1(FPS_60);

	port_tick  = new PORT::Exec_O_Port(this, 0);
	port_delta = new PORT::Data_O_Port(this, 1, DATA::Type::DOUBLE);

	outputs.push_back(port_tick);
	outputs.push_back(port_delta);
}

void EXEC::Tick::exec(const uint16& slot_id) {
	port_tick->exec();
}

CLASS::Data EXEC::Tick::getData(const uint16& slot_id) const {
	return CLASS::Data(*delta, DATA::Type::DOUBLE);
}
