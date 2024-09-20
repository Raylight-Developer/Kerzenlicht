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

	addWidget(tree);
}

GUI::WORKSPACE::Workspace_Shelf::~Workspace_Shelf() {
}

GUI::WORKSPACE::Shelf::Shelf(Workspace_Shelf* parent) :
	Tree(parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);
	FILE->objects.addCallback(this, [this](KL::Observable_Vector_Operation type) {
		f_update();
	});
}

GUI::WORKSPACE::Shelf::~Shelf() {
	FILE->objects.removeCallback(this);
}

void GUI::WORKSPACE::Shelf::f_update() {
	for (const KL::Object* data : FILE->objects) {
		auto item = new Tree_Item(this, QString::fromStdString(data->name), 1);
		item->setData(0, 1000, uptr(data));
		item->setData(0, 1001, "OBJECT");
	}
	for (const KL::OBJECT::Data* data : FILE->object_data) {
		auto item = new Tree_Item(this, QString::fromStdString(data->name), 1);
		item->setData(0, 1000, uptr(data));
		item->setData(0, 1001, "OBJECT_DATA");
	}
	for (const KL::Shader* data : FILE->shaders) {
		auto item = new Tree_Item(this, QString::fromStdString(data->name), 1);
		item->setData(0, 1000, uptr(data));
		item->setData(0, 1001, "SHADER");
	}
	for (const KL::SHADER::Texture* data : FILE->textures) {
		auto item = new Tree_Item(this, QString::fromStdString(data->name), 1);
		item->setData(0, 1000, uptr(data));
		item->setData(0, 1001, "TEXTURE");
	}
}

void GUI::WORKSPACE::Shelf::startDrag(Qt::DropActions actions) {
	QTreeWidgetItem* temp = currentItem();
	if (temp) {
		QByteArray byteArray;
		QDataStream stream(&byteArray, QIODevice::WriteOnly);
		stream << temp->data(0, 1000).toULongLong();

		QMimeData* mimeData = new QMimeData;
		mimeData->setText("POINTER::" + temp->data(0, 1001).toString());
		mimeData->setData(temp->data(0, 1001).toString(), byteArray);

		QDrag* drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->exec(Qt::MoveAction);
	}
}