#include "Workspaces/Properties.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Properties::Workspace_Properties(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	GUI::Tabs* tabs = new GUI::Tabs(this);
	tabs->addTab(new Object_Properties(this), "Object");
	addWidget(tabs);
}

GUI::WORKSPACE::Object_Properties::Object_Properties(Workspace_Properties* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	GUI::Linear_Contents* position_row = new GUI::Linear_Contents(this, QBoxLayout::Direction::TopToBottom);
	GUI::Label* position_label = new GUI::Label(this);
	position_label->setText("Position");
	GUI::Double_Input* x_pos_input = new GUI::Double_Input(this);
	GUI::Double_Input* y_pos_input = new GUI::Double_Input(this);
	GUI::Double_Input* z_pos_input = new GUI::Double_Input(this);
	position_row->addWidget(position_label);
	position_row->addWidget(x_pos_input);
	position_row->addWidget(y_pos_input);
	position_row->addWidget(z_pos_input);

	GUI::Linear_Contents* rotation_row = new GUI::Linear_Contents(this, QBoxLayout::Direction::TopToBottom);
	GUI::Label* rotation_label = new GUI::Label(this);
	rotation_label->setText("Rotation");
	GUI::Double_Input* x_rot_input = new GUI::Double_Input(this);
	GUI::Double_Input* y_rot_input = new GUI::Double_Input(this);
	GUI::Double_Input* z_rot_input = new GUI::Double_Input(this);
	rotation_row->addWidget(rotation_label);
	rotation_row->addWidget(x_rot_input);
	rotation_row->addWidget(y_rot_input);
	rotation_row->addWidget(z_rot_input);

	GUI::Linear_Contents* scale_row = new GUI::Linear_Contents(this, QBoxLayout::Direction::TopToBottom);
	GUI::Label* scale_label = new GUI::Label(this);
	scale_label->setText("Scale");
	GUI::Double_Input* x_scale_input = new GUI::Double_Input(this);
	GUI::Double_Input* y_scale_input = new GUI::Double_Input(this);
	GUI::Double_Input* z_scale_input = new GUI::Double_Input(this);
	scale_row->addWidget(scale_label);
	scale_row->addWidget(x_scale_input);
	scale_row->addWidget(y_scale_input);
	scale_row->addWidget(z_scale_input);

	addWidget(position_row);
	addWidget(rotation_row);
	addWidget(scale_row);
}