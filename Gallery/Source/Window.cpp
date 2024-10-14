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
	KL::Session::getInstance().setLog(log);

	QFontDatabase::addApplicationFont("../Shared/Resources/Fonts/RobotoMono-Medium.ttf");

	setWindowTitle("Kerzenlicht");
	setWindowIcon(QPixmap("./Resources/Icon.png"));

	LOG ENDL << "Gallery 1.0.0 Initialized"; FLUSH;

	tabs = new GUI::Tabs(this);

	auto viewport = new GUI::Graphics_View(this);
	auto scene = new GUI::Graphics_Scene(this);
	viewport->setScene(scene);
	scene->addPixmap(f_showroomThumbnail("./Resources/0.png", "Arona", dvec2(0), dvec2(1), ShowFromat::SQUARE));

	auto showroom = new Showroom(this);

	tabs->addTab(showroom, "ShowRoom");
	tabs->addTab(viewport, "Viewport");

	setCentralWidget(tabs);

	showMaximized();
	LOG ENDL ANSI_G << "Fully Initialized" ANSI_RESET; FLUSH;
}

void GUI::Main_Window::closeEvent(QCloseEvent* event) {
	event->accept();
}