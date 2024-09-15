#include "Main_Window.hpp"

GUI::WORKSPACE::App::App(int argc, char* argv[]) :
	Application(argc, argv)
{
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

bool GUI::WORKSPACE::Main_Window::eventFilter(QObject* object, QEvent* event) {
	if (event->type() == QEvent::MouseMove) {
		if (object->inherits("QOpenGLWindow")) {
		}
	}
	if (!mouse_pressed && event->type() == QEvent::MouseButtonPress) {
		mouse_pressed = true;
		if (object->objectName() == "Viewport_Realtime") {
			QMouseEvent* d_event =static_cast<QMouseEvent*>(event);
			Viewport* viewport = dynamic_cast<Viewport*>(object);
			//viewport->f_selectObject(dvec2(1.0, -1.0) * (dvec2(d_event->pos().x(), d_event->pos().y()) - 1.0 - dvec2(viewport->width(), viewport->height()) / 2.0) / max(i_to_d(viewport->width()), i_to_d(viewport->height())));
		
		}
	}
	if (event->type() == QEvent::MouseButtonRelease) {
		mouse_pressed = false;
	}
	if (!key_pressed && event->type() == QEvent::KeyPress) {
		key_pressed = true;
		if (object->objectName() == "Viewport_Realtime") {
			Viewport* viewport = dynamic_cast<Viewport*>(object);
			QKeyEvent* d_event =static_cast<QKeyEvent*>(event);
			if (d_event->key() == Qt::Key::Key_R) {
				//viewport->f_recompileShaders();
			}
		}
	}
	if (event->type() == QEvent::KeyRelease) {
		key_pressed = false;
	}
	return QObject::eventFilter(object, event);
}

void GUI::WORKSPACE::Main_Window::closeEvent(QCloseEvent* event) {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Exit And Save Changes", "Exit And Save Changes:", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

	if (reply == QMessageBox::Yes) {
		file->f_saveAsciiFile("./Resources/Assets/Save.krz");
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