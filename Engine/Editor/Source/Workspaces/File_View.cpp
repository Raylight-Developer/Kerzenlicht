#include "Workspaces/File_View.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_File_View::Workspace_File_View(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{

	display = new GUI::Text_Stream(this);

	GUI::Linear_Contents* Options = new GUI::Linear_Contents(this, QBoxLayout::Direction::LeftToRight);

	GUI::Button* Export_Button = new GUI::Button(this);
	Export_Button->setText("Refresh");
	connect(Export_Button, &GUI::Button::clicked, [this]() {f_refresh(); });

	Options->addWidget(Export_Button);

	addWidget(Options);
	addWidget(display);
}

void GUI::WORKSPACE::Workspace_File_View::f_refresh() {
	display->clear();
	//display->setText(QString::fromStdString(parent->parent->file->f_printFile()));
}