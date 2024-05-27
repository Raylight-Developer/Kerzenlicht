#include "Workspaces/Log.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Log::Workspace_Log(Workspace_Manager* parent, Lace* log) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent),
	log(log)
{
	GUI::Linear_Contents* header = new GUI::Linear_Contents(this, QBoxLayout::Direction::LeftToRight);
	GUI::Button* clear_button = new GUI::Button(this);
	clear_button->setText("Clear");
	//connect(Clear_Button, &GUI::Button::clicked, [this]() {log->clear(); });

	GUI::Button* export_button = new GUI::Button(this);
	export_button->setText("Export");
	//connect(Export_Button, &GUI::Button::clicked, [this]() {f_exportLog(); });

	header->addWidget(clear_button);
	header->addWidget(export_button);

	addWidget(header);

	GUI::Text_Stream* log_view = new GUI::Text_Stream(this);
	log_view->setText(QString::fromStdString(log->str()));
	addWidget(log_view);
}