#include "Workspaces/History_Manager.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_History_Manager::Workspace_History_Manager(Workspace_Manager* parent) :
	parent(parent),
	Linear_Contents(parent)
{
	list = new List(this);
	addWidget(list);

	KL::Session::getInstance().getHistory()->addCallback(this, [this]() {
		list->addItem(new QListWidgetItem(QString::fromStdString(KL::Session::getInstance().getHistory()->history_stack.top().get()->info)));
	});

	connect(list, &List::itemDoubleClicked, [this]() {
		
	});
}