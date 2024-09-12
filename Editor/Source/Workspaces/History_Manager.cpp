#include "Workspaces/History_Manager.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_History_Manager::Workspace_History_Manager(Workspace_Manager* parent) :
	parent(parent),
	Linear_Contents(parent)
{
	list = new List(this);
	addWidget(list);

	for (const auto& ptr : KL::Session::getInstance().getHistory()->history_stack) {
		const auto command = ptr.get();

		QListWidgetItem* item = new QListWidgetItem();
		item->setData(500, uptr(command));
		item->setText(QString::fromStdString(command->info.str()));
		item->setToolTip(QString::fromStdString(command->details.str()));
		list->addItem(item);
	}

	KL::Session::getInstance().getHistory()->addCallback(this, [this]() {
		KL::History_Command* command = KL::Session::getInstance().getHistory()->history_stack.back().get();
		
		QListWidgetItem* item = new QListWidgetItem();
		item->setData(1000, uptr(command));
		item->setText(QString::fromStdString(command->info.str()));
		item->setToolTip(QString::fromStdString(command->details.str()));
		list->addItem(item);
	});

	connect(list, &List::itemDoubleClicked, [this](QListWidgetItem* item) {
		//KL::History_Command* command = ptr<KL::History_Command*>(item->data(1000).toULongLong());
		const uint64 index = list->indexFromItem(item).row();
		const uint64 count = list->count() - index;
		LOG ENDL << "Undo [" << count << "]"; FLUSH;
		UNDO(count);
	});
}