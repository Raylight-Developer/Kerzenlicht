#include "Workspaces/Shelf.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Shelf::Workspace_Shelf(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	parent->setMaximumWidth(150);
	tree = new Shelf(this);
	objects = new Tree_Item(tree, "Objects");
	object_data = new Tree_Item(tree, "Data");

	for (const CLASS::Object* object : parent->parent->file->objects) {
		auto item = new Tree_Item(objects, QString::fromStdString(object->name), 1);
		item->setData(0, 1000, reinterpret_cast<uint64>(object));
	}
	//for (pair<string, Object_Data*> Mesh : parent->parent->file->data_array[Data_Type::MESH]) {
	//	GUI::Tree_Item* Item = new GUI::Tree_Item(object_data, Mesh.first);
	//}
	addWidget(tree);
}

GUI::WORKSPACE::Shelf::Shelf(Workspace_Shelf* parent) :
	Tree(parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);
}

void GUI::WORKSPACE::Shelf::startDrag(Qt::DropActions actions) {
	QTreeWidgetItem* temp = currentItem();
	if (temp and temp->data(0, 500).toInt() > 0) {
		QByteArray byteArray;
		QDataStream stream(&byteArray, QIODevice::WriteOnly);
		stream << temp->data(0, 1000).toULongLong();

		QMimeData* mimeData = new QMimeData;
		mimeData->setText("OBJECT");
		mimeData->setData("OBJECT", byteArray);

		QDrag* drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->exec(Qt::MoveAction);
	}
}