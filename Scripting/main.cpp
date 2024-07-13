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

		node->data_inputs["I Value"]->default_value = Data(2.5, DATA::Type::DOUBLE);
	}

	EXPORT void Script_ID_exec(EXEC::Script* node) {
		node->exec_outputs["O Exec"]->exec();
	}

	EXPORT Data* Script_ID_getData(const EXEC::Script* node, const uint16& port_request) {
		if (port_request == 1) {
			return &Data(node->data_inputs.at("I Value")->getData() * Data(50.0));
		}
		return &Data();
	}
}