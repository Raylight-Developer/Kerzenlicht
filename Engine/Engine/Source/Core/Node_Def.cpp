#include "Core/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Object/Object.hpp"

using namespace CLASS::NODE;

EXEC::Timer::Timer() {
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::TIMER);
	
	timer.setInterval(50);
	QObject::connect(&timer, &QTimer::timeout, [this]() {exec(""); });
	timer.start();

	port = new PORT::Exec_O_Port(this, "0");
	outputs.push_back(port);
}

void EXEC::Timer::exec(const string& slot_id) {
	port->exec();
}

EXEC::Counter::Counter() {
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::COUNTER);
	count = 0;

	PORT::Exec_I_Port* value = new PORT::Exec_I_Port(this, "0");
	inputs.push_back(value);
}

void EXEC::Counter::exec(const string& slot_id) {
	count++;
}

EXEC::Sequence::Sequence() {
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::SEQUENCE);

	PORT::Exec_I_Port* in = new PORT::Exec_I_Port(this, "0");
	inputs.push_back(in);

	PORT::Exec_O_Port* out = new PORT::Exec_O_Port(this, "0");
	outputs.push_back(out);
}

void EXEC::Sequence::exec(const string& slot_id) {
	for (Port* output : outputs) {
		output->exec();
	}
}

EXEC::Script::Script() {
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::SCRIPT);
	script_id = "Script_ID";

	in = new PORT::Exec_I_Port(this, "0");
	exec_inputs["0"] = in;
	inputs.push_back(in);

	out = new PORT::Exec_O_Port(this, "0");
	exec_outputs["0"] = out;
	outputs.push_back(out);

	loadDLL(dynlib, "DLL.dll");

	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(Script*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (Data(*)(const Script*, const string&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(Script*))buildAddress;
		buildFunc(this);
	}
}


void EXEC::Script::addDataInput(const string& slot_id, const DATA::Type& type, const DATA::Modifier& modifier) {
	PORT::Data_I_Port* value = new PORT::Data_I_Port(this, slot_id, type, modifier);
	data_inputs[slot_id] = value;
	inputs.push_back(value);
	
};

void EXEC::Script::addDataOutput(const string& slot_id, const DATA::Type& type, const DATA::Modifier& modifier) {
	PORT::Data_O_Port* value = new PORT::Data_O_Port(this, slot_id, type, modifier);
	data_outputs[slot_id] = value;
	outputs.push_back(value);
	
};

void EXEC::Script::addExecInput(const string& slot_id) {
	PORT::Exec_I_Port* value = new PORT::Exec_I_Port(this, slot_id);
	exec_inputs[slot_id] = value;
	inputs.push_back(value);
	
};

void EXEC::Script::addExecOutput(const string& slot_id) {
	PORT::Exec_O_Port* value = new PORT::Exec_O_Port(this, slot_id);
	exec_outputs[slot_id] = value;
	outputs.push_back(value);
	
};

void EXEC::Script::reloadFunctions() {
	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(Script*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (Data(*)(const Script*, const string&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(Script*))buildAddress;
		buildFunc(this);
	}
}

void EXEC::Script::reloadDll() {
	unloadDLL(dynlib);
	loadDLL(dynlib, "DLL.dll");

	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(Script*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (Data(*)(const Script*, const string&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(Script*))buildAddress;
		buildFunc(this);
	}
}

void EXEC::Script::clearIO() { // TODO delete GUI connections
	for (auto it = inputs.begin() + 1; it != inputs.end(); ++it) { delete* it; }
	inputs.erase(inputs.begin() + 1, inputs.end());
	for (auto it = outputs.begin() + 1; it != outputs.end(); ++it) { delete* it; }
	outputs.erase(outputs.begin() + 1, outputs.end());

	data_inputs.clear();
	data_outputs.clear();
	exec_inputs.clear();
	exec_outputs.clear();

	in = new PORT::Exec_I_Port(this, "0");
	exec_inputs["0"] = in;
	inputs.push_back(in);

	out = new PORT::Exec_O_Port(this, "0");
	exec_outputs["0"] = out;
	outputs.push_back(out);
}

void EXEC::Script::exec(const string& slot_id) {
	if (execFunc) execFunc(this);
	outputs[0]->exec();
}

Data EXEC::Script::getData(const string& slot_id) const {
	if (getDataFunc) return getDataFunc(this, slot_id);
	return Data();
}

Data EXEC::Script::getInputData(const string& slot_id) const {
	auto it = data_inputs.find(slot_id);
	if (it != data_inputs.end())
		return data_inputs.at(slot_id)->getData();
	return Data();
}

CLASS::NODE::EXEC::Tick::Tick() {
	type = CLASS::NODE::Type::EXEC;
	sub_type = ETOU(CLASS::NODE::EXEC::Type::TICK);
	port_tick = new PORT::Exec_O_Port(this, "0");
	outputs.push_back(port_tick);
	port_delta = new PORT::Data_O_Port(this, "1",  DATA::Type::DOUBLE);
	outputs.push_back(port_delta);
}

void CLASS::NODE::EXEC::Tick::exec(const string& slot_id) {
	port_tick->exec();
}

Data CLASS::NODE::EXEC::Tick::getData(const string& slot_id) const {
	return Data(*delta, DATA::Type::DOUBLE);
}

CLASS::NODE::LINK::Get::Get() {
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::GET);

	port_pointer = new PORT::Data_I_Port(this, "0", DATA::Type::ANY);
	port_output = new PORT::Data_O_Port(this, "0", DATA::Type::ANY);

	inputs.push_back(port_pointer);
	outputs.push_back(port_output);
}

CLASS::NODE::Data CLASS::NODE::LINK::Get::getData(const string& slot_id) const {
	return NODE::Data();
}

CLASS::NODE::LINK::Set::Set() {
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::SET);

	port_pointer = new PORT::Data_I_Port(this, "0", DATA::Type::ANY);
	port_value   = new PORT::Data_I_Port(this, "1", DATA::Type::ANY);
	port_exec_i  = new PORT::Exec_I_Port(this, "0");
	port_exec_o  = new PORT::Exec_O_Port(this, "0");

	inputs.push_back(port_exec_i);
	inputs.push_back(port_pointer);
	outputs.push_back(port_exec_o);
}

void CLASS::NODE::LINK::Set::exec(const string& slot_id) {
	CLASS::NODE::Data data = port_pointer->getData();
	switch (data.type) {
		case CLASS::NODE::DATA::Type::OBJECT: {
			any_cast<CLASS::Object*>(data.data)->transform = any_cast<Transform>(port_value->getData().data);
			break;
		}
	}
	port_exec_o->exec();
}

CLASS::NODE::Data CLASS::NODE::LINK::Set::getData(const string& slot_id) const {
	return NODE::Data();
}

CLASS::NODE::LINK::Pointer::Pointer() {
	type = CLASS::NODE::Type::LINK;
	sub_type = ETOU(CLASS::NODE::LINK::Type::POINTER);

	port = new PORT::Data_O_Port(this, "0", DATA::Type::ANY);
	outputs.push_back(port);
}

CLASS::NODE::Data CLASS::NODE::LINK::Pointer::getData(const string& slot_id) const {
	return NODE::Data(pointer, DATA::Type::SCENE);
}

CLASS::NODE::MATH::MATH::MATH() {
	type = CLASS::NODE::Type::MATH;
	sub_type = ETOU(CLASS::NODE::MATH::Type::ADD);

	inputs .push_back(in_a  = new PORT::Data_I_Port(this, "0", DATA::Type::ANY));
	inputs .push_back(in_b  = new PORT::Data_I_Port(this, "1", DATA::Type::ANY));
	outputs.push_back(out_a = new PORT::Data_O_Port(this, "0", DATA::Type::ANY));
}

CLASS::NODE::Data CLASS::NODE::MATH::Add::getData(const string& slot_id) const {
	return in_a->getData() + in_b->getData();
}
CLASS::NODE::Data CLASS::NODE::MATH::Sub::getData(const string& slot_id) const {
	return in_a->getData() - in_b->getData();
}
CLASS::NODE::Data CLASS::NODE::MATH::Mul::getData(const string& slot_id) const {
	return in_a->getData() * in_b->getData();
}
CLASS::NODE::Data CLASS::NODE::MATH::Div::getData(const string& slot_id) const {
	return in_a->getData() / in_b->getData();
}

CLASS::NODE::UTIL::View::View() {
	type = CLASS::NODE::Type::UTIL;
	sub_type = ETOU(CLASS::NODE::UTIL::Type::VIEW);

	port = new PORT::Data_I_Port(this, "0", DATA::Type::ANY);
	port->default_value = Data(0, DATA::Type::INT);
	inputs.push_back(port);
}

CLASS::NODE::Data CLASS::NODE::UTIL::View::getData(const string& slot_id) const {
	return port->getData();
}