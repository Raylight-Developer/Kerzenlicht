#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Manager::Workspace_Manager(Main_Window* parent, const Workspace_Type& type):
	GUI::Dock(parent),
	parent(parent),
	type(type)
{
	workspace = new GUI::Linear_Contents(this, QBoxLayout::Direction::TopToBottom);

	titlebar = new Workspace_Dock_Header(this, type);
	setTitleBarWidget(titlebar);
	setWindowTitle("Workspace");
}

GUI::WORKSPACE::Workspace_Dock_Header::Workspace_Dock_Header(Workspace_Manager* parent, const Workspace_Type& type) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::LeftToRight),
	parent(parent)
{
	//Expand_Collapse_Toggle->setParent(parent->parent);
	floating_workspace_toggle = new GUI::Floating_Toggle(this);
	floating_workspace_toggle->setObjectName("Icon_Button");
	change_workspace_type = new GUI::Button(this);
	change_workspace_type->setObjectName("Icon_Button");
	expand_settings = new GUI::Toggle(this);
	expand_settings->setObjectName("Icon_Button");
	add_dock = new GUI::Button(this);
	add_dock->setObjectName("Icon_Button");
	add_tab = new GUI::Button(this);
	add_tab->setObjectName("Icon_Button");
	make_floating = new GUI::Toggle(this);
	make_floating->setObjectName("Icon_Button");
	make_collapsible = new GUI::Toggle(this);
	make_collapsible->setObjectName("Icon_Button");
	close_workspace = new GUI::Button(this);
	close_workspace->setObjectName("Icon_Button");

	floating_workspace_toggle->setIcon(QPixmap("./Resources/Icons/Collapse.png"));
	change_workspace_type->setIcon(QPixmap("./Resources/Icons/Workspaces/Dock_Manager.png"));
	expand_settings->setIcon(QPixmap("./Resources/Icons/Settings.png"));
	add_dock->setIcon(QPixmap("./Resources/Icons/Add.png"));
	add_tab->setIcon(QPixmap("./Resources/Icons/Add_Tab.png"));
	make_floating->setIcon(QPixmap("./Resources/Icons/Float_Dock.png"));
	make_collapsible->setIcon(QPixmap("./Resources/Icons/Collapse.png"));
	close_workspace->setIcon(QPixmap("./Resources/Icons/Close.png"));

	connect(floating_workspace_toggle, &GUI::Toggle::clicked, [this](bool checked) {f_expandCollapseToggle(checked); });
	connect(change_workspace_type, &GUI::Button::clicked, [this]() {f_selectWorkspaceTypeClick(); });
	connect(expand_settings, &GUI::Toggle::clicked, [this](bool checked) {f_showHideSettings(checked); });
	connect(add_dock, &GUI::Button::clicked, [this]() {f_addDockClick(); });
	connect(add_tab, &GUI::Button::clicked, [this]() {f_addDockTabClick(); });
	connect(make_floating, &GUI::Toggle::clicked, [this](bool checked) {f_setFloatingToggle(checked); });
	connect(make_collapsible, &GUI::Toggle::clicked, [this](bool checked) {f_setCollapsibleModeToggle(checked); });
	connect(close_workspace, &GUI::Button::clicked, [this]() {f_closeClick(); });

	floating_workspace_toggle->setChecked(true);
	floating_workspace_toggle->setText("Manager");
	addWidget(change_workspace_type);
	addWidget(expand_settings);
	addWidget(add_dock);
	addWidget(add_tab);
	addWidget(make_floating);
	addWidget(make_collapsible);
	addWidget(close_workspace);

	expand_settings->setChecked(false);
	f_showHideSettings(false);
	make_collapsible->setCheckable(false);

	f_initWorkspacesMenu();
	f_initCloseConfirmationMenu();
	f_setWorkspace(type);
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_initWorkspacesMenu() {
	select_workspace_type = new GUI::Menu(this);
	GUI::Linear_Layout* layout = new GUI::Linear_Layout(select_workspace_type, QBoxLayout::Direction::TopToBottom);
	select_workspace_type->setFixedWidth(200);

	GUI::Button* api = new GUI::Button(select_workspace_type);
	api->setObjectName("Option_Button");
	GUI::Button* shelf = new GUI::Button(select_workspace_type);
	shelf->setObjectName("Option_Button");
	GUI::Button* scene_outliner = new GUI::Button(select_workspace_type);
	scene_outliner->setObjectName("Option_Button");
	GUI::Button* properties = new GUI::Button(select_workspace_type);
	properties->setObjectName("Option_Button");
	GUI::Button* object_nodes = new GUI::Button(select_workspace_type);
	object_nodes->setObjectName("Option_Button");
	GUI::Button* shader_editor = new GUI::Button(select_workspace_type);
	shader_editor->setObjectName("Option_Button");
	GUI::Button* viewport = new GUI::Button(select_workspace_type);
	viewport->setObjectName("Option_Button");
	GUI::Button* history = new GUI::Button(select_workspace_type);
	history->setObjectName("Option_Button");
	//GUI::Button* log = new GUI::Button(select_workspace_type);
	//log->setObjectName("Option_Button");

	api->setIcon(QIcon("./Resources/Icons/Workspaces/Api.png"));
	api->setText(" API");
	connect(api, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::API); });

	shelf->setIcon(QIcon("./Resources/Icons/Workspaces/Shelf.png"));
	shelf->setText(" Shelf");
	connect(shelf, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::SHELF); });

	scene_outliner->setIcon(QIcon("./Resources/Icons/Workspaces/Scene.png"));
	scene_outliner->setText(" Scene");
	connect(scene_outliner, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::SCENE_OUTLINER); });

	properties->setIcon(QIcon("./Resources/Icons/Workspaces/Properties.png"));
	properties->setText(" Properties");
	connect(properties, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::PROPERTIES); });
	
	object_nodes->setIcon(QIcon("./Resources/Icons/Workspaces/Node_Editor.png"));
	object_nodes->setText(" Object Nodes");
	connect(object_nodes, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::OBJECT_NODE_EDITOR); });

	shader_editor->setIcon(QIcon("./Resources/Icons/Workspaces/Shader_Editor.png"));
	shader_editor->setText(" Shader Editor");
	connect(shader_editor, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::SHADER_EDITOR); });

	viewport->setIcon(QIcon("./Resources/Icons/Workspaces/Viewport.png"));
	viewport->setText(" Viewport");
	connect(viewport, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::VIEWPORT); });

	history->setIcon(QIcon("./Resources/Icons/Workspaces/Log.png"));
	history->setText(" History");
	connect(history, &GUI::Button::clicked, [this]() {f_setWorkspace(Workspace_Type::HISTORY_MANAGER); });

	layout->addWidget(api);
	layout->addWidget(shelf);
	layout->addWidget(scene_outliner);
	layout->addWidget(properties);
	layout->addWidget(object_nodes);
	layout->addWidget(shader_editor);
	layout->addWidget(viewport);
	layout->addWidget(history);
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_selectWorkspaceTypeClick() {
	select_workspace_type->exec(mapToGlobal(QPoint(change_workspace_type->pos().x(), change_workspace_type->pos().y() + 25)));
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_setWorkspace(const Workspace_Type& type) {
	delete parent->workspace;
	switch (type) {
		case (Workspace_Type::API): {
			parent->setWindowTitle("API");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Api.png"));
			parent->workspace = new Workspace_API(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("API");
			break;
		}
		case (Workspace_Type::SHELF): {
			parent->setWindowTitle("Outliner");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/shelf.png"));
			parent->workspace = new Workspace_Shelf(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Shelf");
			break;
		}
		case (Workspace_Type::SCENE_OUTLINER): {
			parent->setWindowTitle("Outliner");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Scene.png"));
			parent->workspace = new Workspace_Scene_Outliner(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Outliner");
			break;
		}
		case (Workspace_Type::PROPERTIES): {
			parent->setWindowTitle("Properties");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Properties.png"));
			parent->workspace = new Workspace_Properties(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Properties");
			break;
		}
		case (Workspace_Type::OBJECT_NODE_EDITOR): {
			parent->setWindowTitle("Object Nodes");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Node_Editor.png"));
			parent->workspace = new Workspace_Object_Node_Editor(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Object Nodes");
			break;
		}
		case (Workspace_Type::SHADER_EDITOR): {
			parent->setWindowTitle("Shader Editor");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Shader_Editor.png"));
			parent->workspace = new Workspace_Shader_Editor(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Shader Editor");
			break;
		}
		case (Workspace_Type::VIEWPORT): {
			parent->setWindowTitle("Viewport");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Viewport.png"));
			parent->workspace = new Workspace_Viewport(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Viewport");
			break;
		}
		case (Workspace_Type::HISTORY_MANAGER): {
			parent->setWindowTitle("History");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Log.png"));
			parent->workspace = new Workspace_History_Manager(parent);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("History");
			break;
		}
		default: {
			parent->setWindowTitle("Manager");
			change_workspace_type->setIcon(QIcon("./Resources/Icons/Workspaces/Dock_Manager.png"));
			parent->workspace = new GUI::Linear_Contents(this, QBoxLayout::Direction::LeftToRight);
			parent->setWidget(parent->workspace);
			floating_workspace_toggle->setText("Manager");
			break;
		}
	}
	select_workspace_type->close();
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_showHideSettings(bool toggle) {
	if (toggle) {
		add_dock->show();
		add_tab->show();
		make_floating->show();
		make_collapsible->show();
		close_workspace->show();
	}
	else {
		add_dock->hide();
		add_tab->hide();
		make_floating->hide();
		make_collapsible->hide();
		close_workspace->hide();
	}
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_initCloseConfirmationMenu() {
	close_confirmation = new GUI::Menu(this);

	GUI::Button* close_button = new GUI::Button(close_confirmation);
	close_button->setText("Close");
	connect(close_button, &GUI::Button::clicked, [this]() {f_closeDock(); });

	GUI::Button* cancel_button = new GUI::Button(close_confirmation);
	cancel_button->setText("Cancel");
	connect(cancel_button, &GUI::Button::clicked, [this]() {close_confirmation->close(); });

	GUI::Linear_Layout* layout = new GUI::Linear_Layout(close_confirmation);
	layout->addWidget(close_button);
	layout->addWidget(cancel_button);
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_closeClick() {
	const int PosX = mapToGlobal(parent->widget()->geometry().center()).x() - close_confirmation->layout()->sizeHint().width() / 2;
	const int PosY = mapToGlobal(parent->widget()->geometry().center()).y() - close_confirmation->layout()->sizeHint().height() / 2;
	close_confirmation->exec(QPoint(PosX,PosY));
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_closeDock() {
	close_confirmation->close();
	if (parent->windowTitle() == "Log")
		parent->widget()->setParent(parent->parent);
	parent->parent->removeDockWidget(parent);
	floating_workspace_toggle->deleteLater();
	parent->deleteLater();
	deleteLater();
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_addDockClick() {
	Workspace_Manager* dock = new Workspace_Manager(parent->parent, Workspace_Type::NONE);
	parent->parent->addDockWidget(Qt::TopDockWidgetArea, dock);
	dock->setFloating(true);
	dynamic_cast<Workspace_Dock_Header*>(dock->titlebar)->expand_settings->setChecked(true);
	dynamic_cast<Workspace_Dock_Header*>(dock->titlebar)->f_showHideSettings(true);
	dock->setGeometry(QRect(mapToGlobal(QPoint(parent->widget()->geometry().center().x() - 100, parent->widget()->geometry().center().y() - 100)), mapToGlobal(QPoint(parent->widget()->geometry().center().x() + 100, parent->widget()->geometry().center().y() + 100))));
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_addDockTabClick() {
	Workspace_Manager* dock = new Workspace_Manager(parent->parent, Workspace_Type::NONE);
	parent->parent->tabifyDockWidget(parent, dock);
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_setFloatingToggle(bool toggle) {
	if (toggle) {
		//Float
		make_collapsible->setCheckable(true);
		parent->setFloating(true);
		parent->setAllowedAreas(Qt::NoDockWidgetArea);
		make_floating->setIcon(QPixmap("resources/Redock_Dock.png"));
	}
	else {
		//Dock
		make_collapsible->setCheckable(false);
		make_collapsible->setChecked(false);
		f_setCollapsibleModeToggle(false);
		parent->setFloating(false);
		parent->setAllowedAreas(Qt::AllDockWidgetAreas);
		make_floating->setIcon(QPixmap("resources/Float_Dock.png"));
	}
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_setCollapsibleModeToggle(bool toggle) {
	if (toggle) {
		floating_workspace_toggle->show();
		floating_workspace_toggle->setGeometry(QRect(QPoint(mapToGlobal(parent->rect().topLeft()).x() - floating_workspace_toggle->sizeHint().width(), mapToGlobal(parent->rect().topLeft()).y() - floating_workspace_toggle->sizeHint().height()), mapToGlobal(parent->rect().topLeft())));
		//TODO if window closed when button is active it will keep showing
		//TODO alt tab keeps buttons on top
	}
	else {
		floating_workspace_toggle->hide();
	}
}

void GUI::WORKSPACE::Workspace_Dock_Header::f_expandCollapseToggle(bool toggle) {
	if (toggle) { //Expand
		floating_workspace_toggle->setIcon(QPixmap("resources/Collapse.png"));
		parent->show();
		parent->restoreGeometry(floating_geometry_storage);
	}
	else { //Collapse
		floating_workspace_toggle->setIcon(QPixmap("resources/Expand.png"));
		floating_geometry_storage = parent->saveGeometry();
		parent->hide();
	}
}

void GUI::WORKSPACE::Workspace_Dock_Header::mouseDoubleClickEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
	}
}