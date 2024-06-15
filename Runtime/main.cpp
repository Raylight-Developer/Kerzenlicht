#include "DLL.h"

#include "Include.hpp"
#include "Core/Nodes.hpp"
#include "Core/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Node_GUI_Def.hpp"

using namespace CLASS::NODE;

extern "C" {
	EXPORT void Script_ID_buildGui(GUI::NODE::EXEC::Script* node) {
		node->clearIO();
		
		node->addExecInput (0, "I Exec");
		node->addDataInput (1, "I Value", DATA::Type::DOUBLE);
		
		node->addExecOutput(0, "O Exec");
		node->addDataOutput(1, "O Value", DATA::Type::DOUBLE);
	}

	EXPORT void Script_ID_build(EXEC::Script* node) {
		node->clearIO();

		node->addExecInput (0, "I Exec");
		node->addDataInput (1, "I Value", DATA::Type::DOUBLE);

		node->addExecOutput(0, "O Exec");
		node->addDataOutput(1, "O Value", DATA::Type::DOUBLE);

		node->data_inputs[0]->default_value = Data(2.5, DATA::Type::DOUBLE);
	}

	EXPORT void Script_ID_exec(EXEC::Script* node) {
		random_device rd;
		mt19937 gen(rd());

		uniform_int_distribution<> dis(-1000, 0);
		node->internal_data["internal value"] = Data(static_cast<double>(dis(gen)), DATA::Type::DOUBLE);
		node->exec_outputs["O Exec"]->exec();
	}

	EXPORT Data Script_ID_getData(const EXEC::Script* node, const uint16& port_request) {
		if (port_request == 1) {
			auto it = node->internal_data.find("internal value");
			if (it != node->internal_data.end())
				return it->second;
		}
		return Data();
	}
}