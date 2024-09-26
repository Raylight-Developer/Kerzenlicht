#include "Dll_Include.hpp"

BUILD_(Script_ID)(Script* node) {
	node->clearIO();
		
	node->addExecInput("I Exec");
	node->addDataInput("I Value", Prop_Type::DOUBLE);
		
	node->addExecOutput("O Exec");
	node->addDataOutput("O Value", Prop_Type::DOUBLE);
		
	node->getDataInput("I Value")->default_value = Prop(2.5, Prop_Type::DOUBLE);
}

EXEC_(Script_ID)(Script* node) {
	node->getExecOutput("O Exec")->exec();
}

FETCH_(Script_ID)(Script* node, const uint16& port_request) {
	if (port_request == 1) {
		return node->getData("I Value") * Prop(5.0);
	}
	return Prop();
}

F_BUILD_(Ganyu) {
	node->clearIO();

	node->addExecInput("I Exec");
	node->addDataInput("Delta", Prop_Type::DOUBLE);
	node->addExecOutput("O Exec");
}

F_EXEC_(Ganyu) {
	{
		auto confirm = file->f_getObject("Ganyu");
		if (confirm) {
			confirm.data->node_transform.euler_rotation = dvec3(0, file->f_activeScene()->current_frame, 0);
		}
	}
	node->getExecOutput("O Exec")->exec();
	{
		if (!node->hasInternalData("z_distances")) {
			node->setInternalData("z_distances", Prop(
				vector<dvec1>({
					file->f_activeCamera()->getCamera()->lenses[5].z_distance
				}),
				Prop_Type::DOUBLE,
				Prop_Mod::VECTOR
			));
		}
	}
	vector<dvec1> z_distances = node->getInternalData("z_distances")->getDoubleVector();
	
	if (file->f_activeCamera()->getCamera()->lenses.size() > 0) {
		file->f_activeCamera()->getCamera()->lenses[5].z_distance = z_distances[0] * std::sin(session->f_runtime());
	}
}

F_FETCH_(Ganyu) {
	return Prop();
}