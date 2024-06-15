#include "Workspaces/Scene_Outliner.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Scene_Outliner::Workspace_Scene_Outliner(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	tree = new GUI::Tree(this);
	tree->setDragEnabled(true);
	tree->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
	addWidget(tree);

	objects = new GUI::Tree_Item(tree, "Objects");
	object_data = new GUI::Tree_Item(tree, "Data");

	for (const CLASS::Object* object : parent->parent->file->objects) {
		GUI::Tree_Item* Item = new GUI::Tree_Item(objects, QString::fromStdString(object->name));
	}
	//for (pair<string, Object_Data*> Mesh : parent->parent->file->data_array[Data_Type::MESH]) {
	//	GUI::Tree_Item* Item = new GUI::Tree_Item(object_data, Mesh.first);
	//}
}