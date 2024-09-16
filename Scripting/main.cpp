#include "Dll_Include.hpp"

extern "C" {
	//EXPORT void Script_ID_buildGui(Gui_Script* node) {
	//	node->clearIO();
	//
	//	node->addExecInput("I Exec");
	//	node->addDataInput("I Value", Data_Type::DOUBLE);
	//	
	//	node->addExecOutput("O Exec");
	//	node->addDataOutput("O Value", Data_Type::DOUBLE);
	//}

	// Optional
	//EXPORT void Script_ID_renderGui(Gui_Script* node, QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	//	node->renderDefault(painter, option, widget);
	//}

	EXPORT void Script_ID_build(Script* node) {
		node->clearIO();
		
		node->addExecInput("I Exec");
		node->addDataInput("I Value", Prop_Type::DOUBLE);
		
		node->addExecOutput("O Exec");
		node->addDataOutput("O Value", Prop_Type::DOUBLE);
		
		node->getDataInput("I Value")->default_value = Prop(2.5, Prop_Type::DOUBLE);
	}

	EXPORT void Script_ID_exec(Script* node) {
		node->getExecOutput("O Exec")->exec();
	}

	EXPORT Prop Script_ID_getData(Script* node, const uint16& port_request) {
		if (port_request == 1) {
			return node->getData("I Value") * Prop(5.0);
		}
		return Prop();
	}
}