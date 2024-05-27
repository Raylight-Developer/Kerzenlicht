#include "DLL.hpp"

#include "Include.hpp"
#include "Core/Nodes.hpp"
#include "Core/Node_Def.hpp"

using namespace CLASS::NODE;

extern "C" {
	EXPORT void Script_ID_build(EXEC::Script* node) {
		node->clearIO();
		node->addDataInput("A", DATA::Type::DOUBLE);
		node->addDataOutput("A", DATA::Type::DOUBLE);
		node->addExecInput("A");
		node->addExecOutput("A");

		node->data_inputs["A"]->default_value = Data(2.5, DATA::Type::DOUBLE);
	}

	EXPORT void Script_ID_exec(EXEC::Script* node) {
		random_device rd;
		mt19937 gen(rd());

		uniform_int_distribution<> dis(-1000, 0);
		node->internal_data["internal value"] = Data(static_cast<double>(dis(gen)), DATA::Type::DOUBLE);
		node->exec_outputs["A"]->exec();
	}

	EXPORT Data Script_ID_getData(const EXEC::Script* node, const string& port_request) {
		if (port_request == "A") {
			auto it = node->internal_data.find("internal value");
			if (it != node->internal_data.end())
				return it->second;
		}
		return Data();
	}
}