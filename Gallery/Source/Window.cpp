#include "Window.hpp"

GUI::App::App(int argc, char* argv[]) :
	Application(argc, argv)
{
	setStyleSheet(QString::fromStdString(loadFromFile("./Resources/QStylesheet.css")));
	window = new Main_Window(this);
	installEventFilter(window);
}

GUI::Main_Window::Main_Window(GUI::Application* app) :
	GUI::Window(),
	log(nullptr),
	app(app)
{
	log = new KL::Lace();
	CORE::Session::getInstance().setLog(log);
	QFontDatabase::addApplicationFont("../Shared/Resources/Fonts/RobotoMono-Medium.ttf");
	setWindowTitle("Kerzenlicht");
	setWindowIcon(QPixmap("./Resources/Icon.png"));
	LOG ENDL << "Gallery 1.0.0 Initialized"; FLUSH;


	db_thread = new Database_Thread();
	
	auto showroom = new Showroom(this);

	tabs = new GUI::Tabs(this);
	tabs->addTab(showroom, "ShowRoom");
	setCentralWidget(tabs);
	showMaximized();
	LOG ENDL ANSI_G << "Fully Initialized" ANSI_RESET; FLUSH;
}

void GUI::Main_Window::closeEvent(QCloseEvent* event) {
	event->accept();
}