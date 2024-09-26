#include "Dll_Include.hpp"

BUILD_(Script_ID)(Gui_Script* node) {
	node->clearIO();
	
	node->addExecInput("I Exec");
	node->addDataInput("I Value", Prop_Type::DOUBLE);
		
	node->addExecOutput("O Exec");
	node->addDataOutput("O Value", Prop_Type::DOUBLE);
}

// Optional
RENDER_(Script_ID)(Gui_Script* node, QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	node->renderDefault(painter, option, widget);
}

F_BUILD_(Ganyu) {
	node->clearIO();
	
	node->addExecInput("I");
	node->addDataInput("Delta", Prop_Type::DOUBLE);
	node->addExecOutput("O");
}