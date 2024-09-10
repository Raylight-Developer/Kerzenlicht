#include "Object/Nodes/Object_Nodes.hpp"

#include "Object/Nodes/Compiler.hpp"

using namespace KL::NODE;

EXEC::Timer::Timer() {
	type = NODE::Type::EXEC;
	sub_type = e_to_us(Type::TIMER);
	
//timer.setInterval(50);
//QObject::connect(&timer, &QTimer::timeout, [this]() {exec(0); });
//timer.start();

	port = new PORT::Exec_O_Port(this, 0);
	outputs.push_back(port);
}

void EXEC::Timer::exec(const uint16& slot_id) {
	port->exec();
}

EXEC::Counter::Counter() {
	type = NODE::Type::EXEC;
	sub_type = e_to_us(Type::COUNTER);

	count = 0;

	i_exec = new PORT::Exec_I_Port(this, 0);

	o_exec = new PORT::Exec_O_Port(this, 0);
	o_count = new PORT::Data_O_Port(this, 1, KL::DATA::Type::UINT);

	inputs.push_back(i_exec);
	outputs.push_back(o_exec);
	outputs.push_back(o_count);
}

void EXEC::Counter::exec(const uint16& slot_id) {
	count++;
}

EXEC::Sequence::Sequence() {
	type = NODE::Type::EXEC;
	sub_type = e_to_us(Type::SEQUENCE);

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

EXEC::Script::Script(const string& script_id) :
	script_id(script_id)
{
	type = NODE::Type::EXEC;
	sub_type = e_to_us(Type::SCRIPT);

	wrapper = new Script_Node(this);
	getDataFunc = nullptr;
	buildFunc = nullptr;
	execFunc = nullptr;
	dynlib = NULL;

	if (script_id != "") {
		loadDLL(dynlib);

		FARPROC execAddress = GetProcAddress(dynlib, (script_id + "_exec").c_str());
		FARPROC dataAddress = GetProcAddress(dynlib, (script_id + "_getData").c_str());
		FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
		if (execAddress and dataAddress and buildAddress) {
			execFunc = (void(*)(Script_Node*))execAddress;
			getDataFunc = (KL::Data(*)(const Script_Node*, const uint16&))dataAddress;
			buildFunc = (void(*)(Script_Node*))buildAddress;
			buildFunc(wrapper);
		}
		else {
			//LOG ENDL << HTML_RED << "[DLL Binding]" << HTML_RESET << " Unable to resolve Script ID"; FLUSH;
		}
	}
}

KL::NODE::EXEC::Script::~Script() {
	delete wrapper;
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
	FARPROC execAddress  = GetProcAddress(dynlib, (script_id + "_exec").c_str());
	FARPROC dataAddress  = GetProcAddress(dynlib, (script_id + "_getData").c_str());
	FARPROC buildAddress = GetProcAddress(dynlib, (script_id + "_build").c_str());
	if (execAddress and dataAddress and buildAddress) {
		execFunc = (void(*)(Script_Node*))execAddress;
		getDataFunc = (KL::Data(*)(const Script_Node*, const uint16&))dataAddress;
		buildFunc = (void(*)(Script_Node*))buildAddress;
		buildFunc(wrapper);
	}
	else {
		//LOG ENDL << HTML_RED << "[DLL Binding]" << HTML_RESET << " Unable to resolve Script ID"; FLUSH;
	}
}

void EXEC::Script::reloadDll() {
	unloadDLL(dynlib);
	loadDLL(dynlib);
	reloadFunctions();
}

void EXEC::Script::clearIO() {
	for (auto it : inputs) {
		delete it;
	}
	inputs.clear();
	for (auto it : outputs) {
		delete it;
	}
	outputs.clear();

	data_inputs.clear();
	data_outputs.clear();
	exec_inputs.clear();
	exec_outputs.clear();
}

void EXEC::Script::exec(const uint16& slot_id) {
	if (execFunc) {
		execFunc(wrapper);
	}
}

KL::Data EXEC::Script::getData(const uint16& slot_id) const {
	if (getDataFunc) {
		return getDataFunc(wrapper, slot_id);
	}
	return KL::Data();
}

KL::Data EXEC::Script::getPortData(const string& map_name) const {
	auto it = data_inputs.find(map_name);
	if (it != data_inputs.end()) {
		return data_inputs.at(map_name)->getData();
	}
	return KL::Data();
}

void KL::NODE::EXEC::Script::recompile(const HINSTANCE& library) {
	dynlib = library;
	reloadFunctions();
}

KL::NODE::EXEC::Script_Node::Script_Node(Script* node) :
	node(node)
{}
KL::Data KL::NODE::EXEC::Script_Node::getData(const string& map_name) const {
	return node->getPortData(map_name);
}
void KL::NODE::EXEC::Script_Node::addDataInput (const string& map_name, const DATA::Type& type, const DATA::Modifier& modifier) const {
	node->addDataInput (static_cast<uint16>(node->inputs .size()), map_name, type, modifier);
}
void KL::NODE::EXEC::Script_Node::addDataOutput(const string& map_name, const DATA::Type& type, const DATA::Modifier& modifier) const {
	node->addDataOutput(static_cast<uint16>(node->outputs.size()), map_name, type, modifier);
}
void KL::NODE::EXEC::Script_Node::addExecInput (const string& map_name) const {
	node->addExecInput (static_cast<uint16>(node->inputs .size()), map_name);
}
void KL::NODE::EXEC::Script_Node::addExecOutput(const string& map_name) const {
	node->addExecOutput(static_cast<uint16>(node->outputs.size()), map_name);
}
void KL::NODE::EXEC::Script_Node::clearIO() const {
	node->clearIO();
}

PORT::Data_I_Port* KL::NODE::EXEC::Script_Node::getDataInput (const string& map_name) const {
	if (node->data_inputs .find(map_name) != node->data_inputs .end())
		return node->data_inputs .at(map_name);
	return nullptr;
}
PORT::Data_O_Port* KL::NODE::EXEC::Script_Node::getDataOutput(const string& map_name) const {
	if (node->data_outputs.find(map_name) != node->data_outputs.end())
		return node->data_outputs.at(map_name);
	return nullptr;
}
PORT::Exec_I_Port* KL::NODE::EXEC::Script_Node::getExecInput (const string& map_name) const {
	if (node->exec_inputs .find(map_name) != node->exec_inputs .end())
		return node->exec_inputs .at(map_name);
	return nullptr;
}
PORT::Exec_O_Port* KL::NODE::EXEC::Script_Node::getExecOutput(const string& map_name) const {
	if (node->exec_outputs.find(map_name) != node->exec_outputs.end())
		return node->exec_outputs.at(map_name);
	return nullptr;
}

EXEC::Tick::Tick() {
	type = NODE::Type::EXEC;
	sub_type = e_to_us(Type::TICK);

	delta = new dvec1(FPS_60);

	port_tick  = new PORT::Exec_O_Port(this, 0);
	port_delta = new PORT::Data_O_Port(this, 1, DATA::Type::DOUBLE);

	outputs.push_back(port_tick);
	outputs.push_back(port_delta);
}

void EXEC::Tick::exec(const uint16& slot_id) {
	port_tick->exec();
}

KL::Data EXEC::Tick::getData(const uint16& slot_id) const {
	return KL::Data(*delta, DATA::Type::DOUBLE);
}

LINK::Get::Get() {
	type = NODE::Type::LINK;
	sub_type = e_to_us(Type::GET);
	mini_type = GET::Type::NONE;

	i_pointer = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);
	
	o_value   = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(i_pointer);
	outputs.push_back(o_value);
}

LINK::Set::Set() {
	type = NODE::Type::LINK;
	sub_type = e_to_us(Type::SET);
	mini_type = SET::Type::NONE;

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

LINK::GET::Field::Field(const string& field) :
	Get(),
	field(field)
{
	type = NODE::Type::LINK;
	sub_type = e_to_us(LINK::Type::GET);
	mini_type = GET::Type::FIELD;
}

KL::Data LINK::GET::Field::getData(const uint16& slot_id) const {
	KL::Data pointer_ref = i_pointer->getData();
	switch (pointer_ref.type) {
		case DATA::Type::OBJECT: {
			if (field == "transform.euler.x")
				return KL::Data(pointer_ref.getObject()->transform.euler_rotation.x, DATA::Type::DOUBLE);
			if (field == "node_transform.euler.x")
				return KL::Data(pointer_ref.getObject()->node_transform.euler_rotation.x, DATA::Type::DOUBLE);
			break;
		}
		case DATA::Type::SCENE: {
			if (field == "current_frame")
				return KL::Data(pointer_ref.getScene()->current_frame, DATA::Type::INT);
			break;
		}
	}
	return Data();
}

LINK::SET::Euler_Rotation_X::Euler_Rotation_X() {
	mini_type = SET::Type::EULER_ROTATION_X;

	i_pointer->data_type = DATA::Type::OBJECT;
	i_value->data_type = DATA::Type::DOUBLE;
	o_value->data_type = DATA::Type::DOUBLE;
}

void LINK::SET::Euler_Rotation_X::exec(const uint16& slot_id) {
	KL::Data pointer_ref = i_pointer->getData();
	KL::Data value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case DATA::Type::OBJECT: {
			pointer_ref.getObject()->node_transform.euler_rotation.x = value_ref.getDouble();
			break;
		}
	}
}

LINK::SET::Field::Field() :
	Set()
{
	type = NODE::Type::LINK;
	sub_type = e_to_us(LINK::Type::SET);
	mini_type = SET::Type::FIELD;

	field = "";
}

void LINK::SET::Field::exec(const uint16& slot_id) {
	KL::Data pointer_ref = i_pointer->getData();
	KL::Data value_ref = i_value->getData();
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
	sub_type = e_to_us(Type::POINTER);
	pointer_type = DATA::Type::NONE;
	pointer = nullptr;

	o_pointer = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	outputs.push_back(o_pointer);
}

KL::Data LINK::Pointer::getData(const uint16& slot_id) const {
	switch (pointer_type) {
		case DATA::Type::SCENE:
			return KL::Data(static_cast<Scene*>(pointer), DATA::Type::SCENE);
		case DATA::Type::OBJECT:
			return KL::Data(static_cast<Object*>(pointer), DATA::Type::OBJECT);
	}
	return KL::Data();
}

MATH::Math::Math() {
	type = NODE::Type::MATH;
	sub_type = e_to_us(Type::NONE);

	i_value_a   = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);
	i_value_b   = new PORT::Data_I_Port(this, 1, DATA::Type::ANY);

	o_value_res = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(i_value_a);
	inputs.push_back(i_value_b);
	outputs.push_back(o_value_res);
}

KL::NODE::MATH::Add::Add() {
	sub_type = e_to_us(Type::ADD);
}

KL::Data MATH::Add::getData(const uint16& slot_id) const {
	return i_value_a->getData() + i_value_b->getData();
}

KL::NODE::MATH::Sub::Sub() {
	sub_type = e_to_us(Type::SUB);
}
KL::Data MATH::Sub::getData(const uint16& slot_id) const {
	return i_value_a->getData() - i_value_b->getData();
}

KL::NODE::MATH::Mul::Mul() {
	sub_type = e_to_us(Type::MUL);
}
KL::Data MATH::Mul::getData(const uint16& slot_id) const {
	return i_value_a->getData() * i_value_b->getData();
}

KL::NODE::MATH::Div::Div() {
	sub_type = e_to_us(Type::DIV);
}
KL::Data MATH::Div::getData(const uint16& slot_id) const {
	return i_value_a->getData() / i_value_b->getData();
}

KL::NODE::UTIL::Print::Print() {
	type = NODE::Type::UTIL;
	sub_type = e_to_us(Type::PRINT);

	i_exec  = new PORT::Exec_I_Port(this, 0);
	i_value = new PORT::Data_I_Port(this, 1, DATA::Type::ANY);

	o_exec  = new PORT::Exec_O_Port(this, 0);

	inputs.push_back(i_exec);
	inputs.push_back(i_value);

	outputs.push_back(o_exec);
}

void KL::NODE::UTIL::Print::exec(const uint16& slot_id) {
	cout << endl << i_value->getData().to_string();
	o_exec->exec();
}

UTIL::Cast::Cast() {
	type = NODE::Type::UTIL;
	sub_type = e_to_us(Type::CAST);
	mini_type = CAST::Type::NONE;

	i_value = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);

	o_value = new PORT::Data_O_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(i_value);
	outputs.push_back(o_value);
}

KL::NODE::UTIL::CAST::Uint_To_Double::Uint_To_Double() :
	Cast()
{
	mini_type = CAST::Type::UINT_TO_DOUBLE;

	i_value->data_type = DATA::Type::UINT;
	i_value->default_value = Data(0ULL);
	o_value->data_type = DATA::Type::DOUBLE;
}

KL::Data KL::NODE::UTIL::CAST::Uint_To_Double::getData(const uint16& slot_id) const {
	return KL::Data(static_cast<dvec1>(i_value->getData().getUint()));
}

KL::NODE::UTIL::CAST::Int_To_Double::Int_To_Double() :
	Cast()
{
	mini_type = CAST::Type::INT_TO_DOUBLE;

	i_value->data_type = DATA::Type::INT;
	i_value->default_value = Data(0LL);
	o_value->data_type = DATA::Type::DOUBLE;
}

KL::Data KL::NODE::UTIL::CAST::Int_To_Double::getData(const uint16& slot_id) const {
	return KL::Data(static_cast<dvec1>(i_value->getData().getInt()));
}

UTIL::View::View() {
	type = NODE::Type::UTIL;
	sub_type = e_to_us(Type::VIEW);

	port = new PORT::Data_I_Port(this, 0, DATA::Type::ANY);

	inputs.push_back(port);
}

KL::Data UTIL::View::getData(const uint16& slot_id) const {
	return port->getData();
}