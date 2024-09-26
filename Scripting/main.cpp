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
			vector<dvec1> z_distances;
			for (auto lens : file->f_activeCamera()->getCamera()->lenses) {
				z_distances.push_back(lens.z_distance);
			}
			node->setInternalData("z_distances", Prop(
				z_distances,
				Prop_Type::DOUBLE,
				Prop_Mod::VECTOR
			));
		}
	}
	vector<dvec1> z_distances = node->getInternalData("z_distances")->getDoubleVector();
	
	if (file->f_activeCamera()->getCamera()->lenses.size() > 0) {
		for (uint64 i = 2; i <= 15; i++) {
			file->f_activeCamera()->getCamera()->lenses[i].z_distance = z_distances[i] + 2.0 * (std::sin(session->f_runtime()) * 0.5 + 0.5);
		}
	}
}

F_FETCH_(Ganyu) {
	return Prop();
}