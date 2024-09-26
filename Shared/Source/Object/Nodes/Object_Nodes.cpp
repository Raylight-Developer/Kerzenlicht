#include "Object/Nodes/Object_Nodes.hpp"

#include "Utils/Session.hpp"

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
	o_count = new PORT::Data_O_Port(this, 1, KL::PROP::Type::UINT);

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
			execFunc = (void(*)(Script_Node*, File*, Session*, Lace*))execAddress;
			getDataFunc = (KL::Prop(*)(Script_Node*, const uint16&))dataAddress;
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

void EXEC::Script::addDataInput(const uint16& slot_id, const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier) {
	PORT::Data_I_Port* value = new PORT::Data_I_Port(this, slot_id, type, modifier);
	data_inputs[map_name] = value;
	inputs.push_back(value);
};

void EXEC::Script::addDataOutput(const uint16& slot_id, const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier) {
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
		execFunc = (void(*)(Script_Node*, File*, Session*, Lace*))execAddress;
		getDataFunc = (KL::Prop(*)(Script_Node*, const uint16&))dataAddress;
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
		execFunc(wrapper, FILE, &KL::Session::getInstance(), KL::Session::getInstance().getLog());
	}
}

KL::Prop EXEC::Script::getData(const uint16& slot_id) const {
	if (getDataFunc) {
		return getDataFunc(wrapper, slot_id);
	}
	return KL::Prop();
}

KL::Prop EXEC::Script::getPortData(const string& map_name) const {
	auto it = data_inputs.find(map_name);
	if (it != data_inputs.end()) {
		return data_inputs.at(map_name)->getData();
	}
	return KL::Prop();
}

void KL::NODE::EXEC::Script::recompile(const HINSTANCE& library) {
	dynlib = library;
	reloadFunctions();
}

KL::NODE::EXEC::Script_Node::Script_Node(Script* node) :
	node(node)
{}
KL::Prop KL::NODE::EXEC::Script_Node::getData(const string& map_name) const {
	return node->getPortData(map_name);
}
void KL::NODE::EXEC::Script_Node::setInternalData(const string& ID, const Prop& data) const {
	node->internal_data[ID] = data;
}
bool KL::NODE::EXEC::Script_Node::hasInternalData(const string& ID) const {
	return f_hasKey(node->internal_data, ID);
}
Prop* KL::NODE::EXEC::Script_Node::getInternalData(const string& ID) const {
	return &node->internal_data[ID];
}
void KL::NODE::EXEC::Script_Node::addDataInput (const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier) const {
	node->addDataInput (static_cast<uint16>(node->inputs .size()), map_name, type, modifier);
}
void KL::NODE::EXEC::Script_Node::addDataOutput(const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier) const {
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
	port_delta = new PORT::Data_O_Port(this, 1, PROP::Type::DOUBLE);

	outputs.push_back(port_tick);
	outputs.push_back(port_delta);
}

void EXEC::Tick::exec(const uint16& slot_id) {
	port_tick->exec();
}

KL::Prop EXEC::Tick::getData(const uint16& slot_id) const {
	return KL::Prop(*delta, PROP::Type::DOUBLE);
}

LINK::Get::Get() {
	type = NODE::Type::LINK;
	sub_type = e_to_us(Type::GET);
	mini_type = GET::Type::NONE;

	i_pointer = new PORT::Data_I_Port(this, 0, PROP::Type::ANY);
	
	o_value   = new PORT::Data_O_Port(this, 0, PROP::Type::ANY);

	inputs.push_back(i_pointer);
	outputs.push_back(o_value);
}

LINK::Set::Set() {
	type = NODE::Type::LINK;
	sub_type = e_to_us(Type::SET);
	mini_type = SET::Type::NONE;

	i_exec    = new PORT::Exec_I_Port(this, 0);
	i_pointer = new PORT::Data_I_Port(this, 1, PROP::Type::ANY);
	i_value   = new PORT::Data_I_Port(this, 2, PROP::Type::ANY);

	o_exec    = new PORT::Exec_O_Port(this, 0);
	o_value   = new PORT::Data_O_Port(this, 1, PROP::Type::ANY);

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

KL::Prop LINK::GET::Field::getData(const uint16& slot_id) const {
	KL::Prop pointer_ref = i_pointer->getData();
	switch (pointer_ref.type) {
		case PROP::Type::OBJECT: {
			if (field == "transform.euler.x")
				return KL::Prop(pointer_ref.getObject()->transform.euler_rotation.x, PROP::Type::DOUBLE);
			if (field == "node_transform.euler.x")
				return KL::Prop(pointer_ref.getObject()->node_transform.euler_rotation.x, PROP::Type::DOUBLE);
			if (field == "node_transform.euler.y")
				return KL::Prop(pointer_ref.getObject()->node_transform.euler_rotation.y, PROP::Type::DOUBLE);
			if (field == "node_transform.euler.z")
				return KL::Prop(pointer_ref.getObject()->node_transform.euler_rotation.z, PROP::Type::DOUBLE);
			break;
		}
		case PROP::Type::SCENE: {
			if (field == "current_frame")
				return KL::Prop(pointer_ref.getScene()->current_frame, PROP::Type::INT);
			break;
		}
	}
	return Prop();
}

LINK::SET::Euler_Rotation_X::Euler_Rotation_X() {
	mini_type = SET::Type::EULER_ROTATION_X;

	i_pointer->data_type = PROP::Type::OBJECT;
	i_value->data_type = PROP::Type::DOUBLE;
	o_value->data_type = PROP::Type::DOUBLE;
}

void LINK::SET::Euler_Rotation_X::exec(const uint16& slot_id) {
	KL::Prop pointer_ref = i_pointer->getData();
	KL::Prop value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case PROP::Type::OBJECT: {
			pointer_ref.getObject()->node_transform.euler_rotation.x = value_ref.getDouble();
			break;
		}
	}
}

LINK::SET::Euler_Rotation_Y::Euler_Rotation_Y() {
	mini_type = SET::Type::EULER_ROTATION_Y;

	i_pointer->data_type = PROP::Type::OBJECT;
	i_value->data_type = PROP::Type::DOUBLE;
	o_value->data_type = PROP::Type::DOUBLE;
}

void LINK::SET::Euler_Rotation_Y::exec(const uint16& slot_id) {
	KL::Prop pointer_ref = i_pointer->getData();
	KL::Prop value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case PROP::Type::OBJECT: {
			pointer_ref.getObject()->node_transform.euler_rotation.y = value_ref.getDouble();
			break;
		}
	}
}

LINK::SET::Euler_Rotation_Z::Euler_Rotation_Z() {
	mini_type = SET::Type::EULER_ROTATION_Z;

	i_pointer->data_type = PROP::Type::OBJECT;
	i_value->data_type = PROP::Type::DOUBLE;
	o_value->data_type = PROP::Type::DOUBLE;
}

void LINK::SET::Euler_Rotation_Z::exec(const uint16& slot_id) {
	KL::Prop pointer_ref = i_pointer->getData();
	KL::Prop value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case PROP::Type::OBJECT: {
			pointer_ref.getObject()->node_transform.euler_rotation.z = value_ref.getDouble();
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
	KL::Prop pointer_ref = i_pointer->getData();
	KL::Prop value_ref = i_value->getData();
	switch (pointer_ref.type) {
		case PROP::Type::OBJECT: {
			switch (value_ref.type) {
				case PROP::Type::TRANSFORM: {
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
	pointer_type = PROP::Type::NONE;
	pointer = nullptr;

	o_pointer = new PORT::Data_O_Port(this, 0, PROP::Type::ANY);

	outputs.push_back(o_pointer);
}

KL::Prop LINK::Pointer::getData(const uint16& slot_id) const {
	switch (pointer_type) {
		case PROP::Type::SCENE:
			return KL::Prop(static_cast<Scene*>(pointer), PROP::Type::SCENE);
		case PROP::Type::OBJECT:
			return KL::Prop(static_cast<Object*>(pointer), PROP::Type::OBJECT);
	}
	return KL::Prop();
}

KL::NODE::UTIL::Print::Print() {
	type = NODE::Type::UTIL;
	sub_type = e_to_us(Type::PRINT);

	i_exec  = new PORT::Exec_I_Port(this, 0);
	i_value = new PORT::Data_I_Port(this, 1, PROP::Type::ANY);

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

	i_value = new PORT::Data_I_Port(this, 0, PROP::Type::ANY);

	o_value = new PORT::Data_O_Port(this, 0, PROP::Type::ANY);

	inputs.push_back(i_value);
	outputs.push_back(o_value);
}

KL::NODE::UTIL::CAST::Uint_To_Double::Uint_To_Double() :
	Cast()
{
	mini_type = CAST::Type::UINT_TO_DOUBLE;

	i_value->data_type = PROP::Type::UINT;
	i_value->default_value = Prop(0ULL);
	o_value->data_type = PROP::Type::DOUBLE;
}

KL::Prop KL::NODE::UTIL::CAST::Uint_To_Double::getData(const uint16& slot_id) const {
	return KL::Prop(static_cast<dvec1>(i_value->getData().getUint()));
}

KL::NODE::UTIL::CAST::Int_To_Double::Int_To_Double() :
	Cast()
{
	mini_type = CAST::Type::INT_TO_DOUBLE;

	i_value->data_type = PROP::Type::INT;
	i_value->default_value = Prop(0LL);
	o_value->data_type = PROP::Type::DOUBLE;
}

KL::Prop KL::NODE::UTIL::CAST::Int_To_Double::getData(const uint16& slot_id) const {
	return KL::Prop(static_cast<dvec1>(i_value->getData().getInt()));
}

UTIL::View::View() {
	type = NODE::Type::UTIL;
	sub_type = e_to_us(Type::VIEW);

	port = new PORT::Data_I_Port(this, 0, PROP::Type::ANY);

	inputs.push_back(port);
}

KL::Prop UTIL::View::getData(const uint16& slot_id) const {
	return port->getData();
}