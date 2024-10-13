#include "Main_Window.hpp"

GUI::WORKSPACE::App::App(int argc, char* argv[]) :
	Application(argc, argv)
{
	setStyleSheet(QString::fromStdString(loadFromFile("./Resources/QStylesheet.css")));
	window = new Main_Window(this);
	installEventFilter(window);
}

GUI::WORKSPACE::Main_Window::Main_Window(GUI::Application* app) :
	GUI::Window(),
	log(nullptr),
	file(nullptr),
	history(nullptr),
	app(app),
	key_pressed(false),
	mouse_pressed(false)
{
	log = new KL::Lace();
	KL::Session::getInstance().setLog(log);
	file = new KL::Editor_File();
	KL::Session::getInstance().setFile(file);
	history = new KL::History_Manager();
	KL::Session::getInstance().setHistory(history);

	QFontDatabase::addApplicationFont("../Shared/Resources/Fonts/RobotoMono-Medium.ttf");

	setWindowTitle("Kerzenlicht");
	setWindowIcon(QPixmap("./Resources/Icon.png"));

	LOG ENDL << "Kerzenlicht 1.0.0 Initialized"; FLUSH;

	file->f_loadAsciiFile("../Shared/Resources/Assets/Ganyu.krz");

	Workspace_Manager* ws_1 = new Workspace_Manager(this, Workspace_Type::OBJECT_NODE_EDITOR);
	Workspace_Manager* ws_2 = new Workspace_Manager(this, Workspace_Type::SHELF);
	Workspace_Manager* ws_3 = new Workspace_Manager(this, Workspace_Type::VIEWPORT);

	workspaces["1"] = ws_1;
	workspaces["2"] = ws_2;
	workspaces["3"] = ws_3;

	Workspace_Header* Toolbar = new Workspace_Header(this);

	addToolBar(Toolbar);

	setCentralWidget(ws_2);
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, ws_1);
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, ws_3);

	showMaximized();

	LOG ENDL ANSI_G << "Fully Initialized" ANSI_RESET; FLUSH;
}

void GUI::WORKSPACE::Main_Window::closeEvent(QCloseEvent* event) {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Exit And Save Changes", "Exit And Save Changes:", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

	if (reply == QMessageBox::Yes) {
		file->f_saveAsciiFile("../Shared/Resources/Assets/Save.krz");
		delete log;
		event->accept();
	}
	else if (reply == QMessageBox::No) {
		delete log;
		event->accept();
	}
	else {
		event->ignore();
	}
}