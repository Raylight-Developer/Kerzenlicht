#include "Dll_Include.hpp"

extern "C" {
	EXPORT void Script_ID_buildGui(Gui_Script* node) {
		node->clearIO();

		node->addExecInput(0, "I Exec");
		node->addDataInput(1, "I Value", Data_Type::DOUBLE);
		
		node->addExecOutput(0, "O Exec");
		node->addDataOutput(1, "O Value", Data_Type::DOUBLE);
	}

	EXPORT void Script_ID_build(Script* node) {
		node->clearIO();
		
		node->addExecInput(0, "I Exec");
		node->addDataInput(1, "I Value", Data_Type::DOUBLE);
		
		node->addExecOutput(0, "O Exec");
		node->addDataOutput(1, "O Value", Data_Type::DOUBLE);
		
		node->getDataInput("I Value")->default_value = Data(2.5, Data_Type::DOUBLE);
	}

	EXPORT void Script_ID_exec(Script* node) {
		node->getExecOutput("O Exec")->exec();
	}

	EXPORT Data Script_ID_getData(Script* node, const uint16& port_request) {
		if (port_request == 1) {
			return node->getData("I Value") * Data(5.0);
		}
		return Data();
	}
}