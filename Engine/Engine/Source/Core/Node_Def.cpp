#include "Core/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Object/Object.hpp"

using namespace CLASS::NODE;

EXEC::Timer::Timer() {
	type = NODE::Type::EXEC;
	sub_type = ETOU(Type::TIMER);
	
	timer.setInterval(50);
	QObject::connect(&timer, &QTimer::timeout, [this]() {exec(0); });
	timer.start();

	port = new PORT::Exec_O_Port(this, 0);
	outputs.push_back(port);
}

void EXEC::Timer::exec(const uint16& slot_id) {
	port->exec();
}

EXEC::Counter::Counter() {
	type = NODE::Type::EXEC;
	sub_type = ETOU(Type::COUNTER);

	count = 0;

	i_exec = new PORT::Exec_I_Port(this, 0);

	o_exec = new PORT::Exec_O_Port(this, 0);
	o_count = new PORT::Data_O_Port(this, 1, CLASS::NODE::DATA::Type::UINT);

	inputs.push_back(i_exec);
	outputs.push_back(o_exec);
	outputs.push_back(o_count);
}

void EXEC::Counter::exec(const uint16& slot_id) {
	count++;
}

EXEC::Sequence::Sequence() {
	type = NODE::Type::EXEC;
	sub_type = ETOU(Type::SEQUENCE);

	count = 0;

	PORT::Exec_I_Port* in = new PORT::Exec_I_Port(this, 0);
	PORT::Exec_O_Port* out = new PORT::Exec_O_Port(this, 0);

	inputs.push_back(in);
	outputs.push_back(out);
}

void EXEC::Sequence::exec(const uint16& slot_id) {
	for (Port* output : outputs) {
		output->exec();
	}
}

EXEC::Script::Script() {
	type = NODE::Type::EXEC;
	sub_type = ETOU(Type::SCRIPT);
	script_id = "Script_ID";

	getDataFunc = nullptr;
	buildFunc = nullptr;
	execFunc = nullptr;

	loadDLL(dynlib, "D:/Kerzenlicht Renderer/x64/Debug/Runtime.dll");

	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(Script*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (Data(*)(const Script*, const uint16&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(Script*))buildAddress;
		buildFunc(this);
	}
}

void EXEC::Script::addDataInput(const uint16& slot_id, const string& map_name, const DATA::Type& type, const DATA::Modifier& modifier) {
	PORT::Data_I_Port* value = new PORT::Data_I_Port(this, slot_id, type, modifier);
	data_inputs[map_name] = value;
	inputs.push_back(value);
};

void EXEC::Script::addDataOutput(const uint16& slot_id, const string& map_name, const DATA::Type& type, const DATA::Modifier& modifier) {
	PORT::Data_O_Port* value = new PORT::Data_O_Port(this, slot_id, type, modifier);
	data_outputs[map_name] = value;
	outputs.push_back(value);
};

void EXEC::Script::addExecInput(const uint16& slot_id, const string& map_name) {
	PORT::Exec_I_Port* value = new PORT::Exec_I_Port(this, slot_id);
	exec_inputs[map_name] = value;
	inputs.push_back(value);
};

void EXEC::Script::addExecOutput(const uint16& slot_id, const string& map_name) {
	PORT::Exec_O_Port* value = new PORT::Exec_O_Port(this, slot_id);
	exec_outputs[map_name] = value;
	outputs.push_back(value);
};

void EXEC::Script::reloadFunctions() {
	FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	if (execAddress != NULL) {
		execFunc = (void(*)(Script*))execAddress;
	}
	FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	if (dataAddress != NULL) {
		getDataFunc = (Data(*)(const Script*, const uint16&))dataAddress;
	}
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (buildAddress != NULL) {
		buildFunc = (void(*)(Script*))buildAddress;
		buildFunc(this);
	}
}

void EXEC::Script::reloadDll() {
	unloadDLL(dynlib);
	loadDLL(dynlib, "D:/Kerzenlicht Renderer/x64/Debug/Runtime.dll");
	reloadFunctions();
}

void EXEC::Script::clearIO() { // TODO delete GUI connections
	for (auto it = inputs.begin(); it != inputs.end(); ++it) { delete* it; }
	inputs.erase(inputs.begin(), inputs.end());
	for (auto it = outputs.begin(); it != outputs.end(); ++it) { delete* it; }
	outputs.erase(outputs.begin(), outputs.end());

	data_inputs.clear();
	data_outputs.clear();
	exec_inputs.clear();
	exec_outputs.clear();
}

void EXEC::Script::exec(const uint16& slot_id) {
	if (execFunc) execFunc(this);
	outputs[0]->exec();
}

Data EXEC::Script::getData(const uint16& slot_id) const {
	if (getDataFunc) return getDataFunc(this, slot_id);
	return Data();
}

Data EXEC::Script::getInputData(const string& map_name) const {
	auto it = data_inputs.find(map_name);
	if (it != data_inputs.end())
		return data_inputs.at(map_name)->getData();
	return Data();
}

EXEC::Tick::Tick() {
	type = NODE::Type::EXEC;
	sub_type = ETOU(Type::TICK);

	delta = new dvec1(FPS_60);

	port_tick  = new PORT::Exec_O_Port(this, 0);
	port_delta = new PORT::Data_O_Port(this, 1,  DATA::Type::DOUBLE);

	outputs.push_back(port_tick);
	outputs.push_back(port_delta);
}

void EXEC::Tick::exec(const uint16& slot_id) {
	port_tick->exec();
}

Data EXEC::Tick::getData(const uint16& slot_id) const {
	return Data(*delta, DATA::Type::DOUBLE);
}

LINK::Get::Get() {
	type = NODE::Type::LINK;
	sub_type = ETOU(Type::GET);
	micro_type = GET::Type::NONE;

	i_pointer = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);
	
	o_value   = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(i_pointer);
	outputs.push_back(o_value);
}

LINK::Set::Set() {
	type = NODE::Type::LINK;
	sub_type = ETOU(Type::SET);
	micro_type = SET::Type::NONE;

	i_exec    = new PORT::Exec_I_Port(this, 0);
	i_pointer = new PORT::Data_I_Port(this, 1, DATA::Type::ANY);
	i_value   = new PORT::Data_I_Port(this, 2, DATA::Type::ANY);

	o_exec    = new PORT::Exec_O_Port(this, 0);
	o_value   = new PORT::Data_O_Port(this, 1, DATA::Type::ANY);

	inputs.push_back(i_exec);
	inputs.push_back(i_pointer);
	inputs.push_back(i_value);
	outputs.push_back(o_exec);
	outputs.push_back(o_value);
}

LINK::GET::Field::Field() {
	field = "";
}

Data LINK::GET::Field::getData(const uint16& slot_id) const {
	return NODE::Data();
}

LINK::SET::Euler_Rotation_X::Euler_Rotation_X() {
	i_pointer->data_type = DATA::Type::OBJECT;
	i_value->data_type = DATA::Type::DOUBLE;
	o_value->data_type = DATA::Type::DOUBLE;
}

void LINK::SET::Euler_Rotation_X::exec(const uint16& slot_id) {
	Data pointer_ref = i_pointer->getData();
	Data value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case DATA::Type::OBJECT: {
			if (value_ref.type == DATA::Type::DOUBLE) {
				any_cast<Object*>(pointer_ref.data)->transform.euler_rotation.x += any_cast<double>(value_ref.data);
				//cout << any_cast<Object*>(pointer_ref.data)->name << " Euler Rot X = " << any_cast<Object*>(pointer_ref.data)->transform.euler_rotation.x << endl;
			}
			else if (value_ref.type == DATA::Type::UINT) {
				any_cast<Object*>(pointer_ref.data)->transform.euler_rotation.x += static_cast<double>(any_cast<uint64>(value_ref.data));
				//cout << any_cast<Object*>(pointer_ref.data)->name << " Euler Rot X = " << any_cast<Object*>(pointer_ref.data)->transform.euler_rotation.x << endl;
			}
			break;
		}
	}
}

LINK::SET::Field::Field() {
	field = "";
}

void LINK::SET::Field::exec(const uint16& slot_id) {
	Data pointer_ref = i_pointer->getData();
	Data value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case DATA::Type::OBJECT: {
			switch (value_ref.type) {
				case DATA::Type::TRANSFORM: {
					auto object = any_cast<Object*>(pointer_ref.data);
					auto transform = any_cast<Transform>(value_ref.data);

					if      (field == "")
						object->transform = transform;

					else if (field == "pos")
						object->transform.position = transform.position;
					else if (field == "euler_rot")
						object->transform.euler_rotation = transform.euler_rotation;
					else if (field == "scale")
						object->transform.scale = transform.scale;

					else if (field == "pos.x")
						object->transform.position.x = transform.position.x;
					else if (field == "pos.y")
						object->transform.position.y = transform.position.y;
					else if (field == "pos.z")
						object->transform.position.z = transform.position.z;

					else if (field == "euler_rot.x")
						object->transform.euler_rotation.x = transform.euler_rotation.x;
					else if (field == "euler_rot.y")
						object->transform.euler_rotation.y = transform.euler_rotation.y;
					else if (field == "euler_rot.z")
						object->transform.euler_rotation.z = transform.euler_rotation.z;

					else if (field == "scale.x")
						object->transform.scale.x = transform.scale.x;
					else if (field == "scale.y")
						object->transform.scale.y = transform.scale.y;
					else if (field == "scale.z")
						object->transform.scale.z = transform.scale.z;
					break;
				}
			}
			break;
		}
	}
	o_exec->exec();
}

LINK::Pointer::Pointer() {
	type = NODE::Type::LINK;
	sub_type = ETOU(Type::POINTER);
	pointer_type = DATA::Type::NONE;
	pointer = nullptr;

	o_pointer = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	outputs.push_back(o_pointer);
}

Data LINK::Pointer::getData(const uint16& slot_id) const {
	switch (pointer_type) {
		case DATA::Type::SCENE:
			return NODE::Data(static_cast<Scene*>(pointer), DATA::Type::SCENE);
		case DATA::Type::OBJECT:
			return NODE::Data(static_cast<Object*>(pointer), DATA::Type::OBJECT);
	}
	return NODE::Data();
}

MATH::MATH::MATH() {
	type = NODE::Type::MATH;
	sub_type = ETOU(Type::ADD);

	i_value_a   = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);
	i_value_b   = new PORT::Data_I_Port(this, 1, DATA::Type::ANY);

	o_value_res = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(i_value_a);
	inputs.push_back(i_value_b);
	outputs.push_back(o_value_res);
}

Data MATH::Add::getData(const uint16& slot_id) const {
	return i_value_a->getData() + i_value_b->getData();
}
Data MATH::Sub::getData(const uint16& slot_id) const {
	return i_value_a->getData() - i_value_b->getData();
}
Data MATH::Mul::getData(const uint16& slot_id) const {
	return i_value_a->getData() * i_value_b->getData();
}
Data MATH::Div::getData(const uint16& slot_id) const {
	return i_value_a->getData() / i_value_b->getData();
}

UTIL::Convert::Convert() {
	type = NODE::Type::UTIL;
	sub_type = ETOU(Type::CONVERT);

	i_value = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);

	o_value = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(i_value);
	outputs.push_back(o_value);
}

CLASS::NODE::UTIL::CONVERT::Uint_To_Double::Uint_To_Double() {
	i_value->data_type = DATA::Type::UINT;
	o_value->data_type = DATA::Type::DOUBLE;
}

Data CLASS::NODE::UTIL::CONVERT::Uint_To_Double::getData(const uint16& slot_id) const {
	return Data(static_cast<double>(any_cast<uint64>(i_value->getData().data)), DATA::Type::DOUBLE);
}

UTIL::View::View() {
	type = NODE::Type::UTIL;
	sub_type = ETOU(Type::VIEW);

	port = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(port);
}

Data UTIL::View::getData(const uint16& slot_id) const {
	return port->getData();
}