 #include "pch.h"
#include "Include.hpp"
#include "Node_Def.hpp"

using namespace std;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" {
	SCRIPT void Script_ID_build(SCRIPT_Node* node) {
		node->clearIO(); // Deletes User defined ports and connections

		node->addDataInput ("A", "In  A");
		node->data_inputs["A"]->default_value = 2.5;
		node->addDataOutput("DATA OUT A", "Out A");
		node->addExecOutput("EXEC OUT SECONDARY", "In  A");
		node->addExecInput ("A", "Out A");
	}

	SCRIPT void Script_ID_exec(SCRIPT_Node* node) {
		random_device rd;
		mt19937 gen(rd());

		uniform_int_distribution<> dis(-1000, 0);
		node->internal_data["internal value"] = static_cast<double>(dis(gen));
		node->exec_outputs["EXEC OUT SECONDARY"]->exec();
	}

	SCRIPT any Script_ID_getData(const SCRIPT_Node* node, const string& port_request) {
		if (port_request == "DATA OUT A") {
			auto it = node->internal_data.find("internal value");
			if (it != node->internal_data.end())
				return it->second;
		}
		return 0.0;
	}
}