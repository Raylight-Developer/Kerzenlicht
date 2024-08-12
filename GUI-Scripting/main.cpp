#include "Dll_Include.hpp"

extern "C" {
	EXPORT void Script_ID_buildGui(Gui_Script* node) {
		node->clearIO();
	
		node->addExecInput("I Exec");
		node->addDataInput("I Value", Data_Type::DOUBLE);
		
		node->addExecOutput("O Exec");
		node->addDataOutput("O Value", Data_Type::DOUBLE);
	}

	// Optional
	EXPORT void Script_ID_renderGui(Gui_Script* node, QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
		node->renderDefault(painter, option, widget);
	}
}