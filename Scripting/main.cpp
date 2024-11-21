#include "Dll_Include.hpp"

BUILD_(Script_ID)(Script* NODE) {
	NODE->clearIO();
		
	NODE->addExecInput("I Exec");
	NODE->addDataInput("I Value", Prop_Type::DOUBLE);
		
	NODE->addExecOutput("O Exec");
	NODE->addDataOutput("O Value", Prop_Type::DOUBLE);
		
	NODE->getDataInput("I Value")->default_value = Prop(2.5, Prop_Type::DOUBLE);
}

EXEC_(Script_ID)(Script* NODE) {
	NODE->getExecOutput("O Exec")->exec();
}

FETCH_(Script_ID)(Script* NODE, const uint16& PORT) {
	if (PORT == 1) {
		return NODE->getData("I Value") * Prop(5.0);
	}
	return Prop();
}

F_BUILD_(Ganyu) {
	NODE->clearIO();

	NODE->addExecInput("I Exec");
	NODE->addDataInput("Delta", Prop_Type::DOUBLE);
	NODE->addExecOutput("O Exec");
}

F_EXEC_(Ganyu) {
	{
		auto confirm = FILE->f_getObject("Ganyu");
		if (confirm) {
			confirm.data->node_transform.euler_rotation = dvec3(0, FILE->f_activeScene()->current_frame, 0);
		}
	}
	NODE->getExecOutput("O Exec")->exec();
	{
		if (!NODE->hasInternalData("z_distances")) {
			vector<dvec1> z_distances;
			for (auto lens : FILE->f_activeCamera()->getCamera()->lenses) {
				z_distances.push_back(lens.z_distance);
			}
			NODE->setInternalData("z_distances", Prop(
				z_distances,
				Prop_Type::DOUBLE,
				Prop_Mod::VECTOR
			));
		}
	}
	vector<dvec1> z_distances = NODE->getInternalData("z_distances")->getDoubleVector();
	
	if (FILE->f_activeCamera()->getCamera()->lenses.size() > 0) {
		for (uint64 i = 2; i <= 15; i++) {
			FILE->f_activeCamera()->getCamera()->lenses[i].z_distance = z_distances[i] + 2.0 * (std::sin(SESSION->f_runtime()) * 0.5 + 0.5);
		}
	}
}

F_FETCH_(Ganyu) {
	return Prop();
}