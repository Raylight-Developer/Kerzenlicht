#include "Window.hpp"

GUI::App::App(int argc, char* argv[]) :
	Application(argc, argv)
{
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

	LOG ENDL << "Kerzenlicht 1.0.0 Initialized"; FLUSH;

	showMaximized();

	LOG ENDL ANSI_G << "Fully Initialized" ANSI_RESET; FLUSH;
}

void GUI::Main_Window::closeEvent(QCloseEvent* event) {
	//if (!app->database_thread->work_queue.isEmpty() || !app->online_download_threads.isEmpty()) {
	//	for (Online_Download_Thread* thread : app->online_download_threads) {
	//		app->log->append(qstr("Still Downloading: " + thread->work.params.at("ID")));
	//	}
	//	for (const Database_Query& work : app->database_thread->work_queue) {
	//		app->log->append(qstr("Still Querying: " + work.params.at("Query")));
	//	}
	//	QMessageBox::warning(this, "Confirmation", "Threads haven't finished processing");
	//	event->ignore();
	//	return;
	//}
	//if (PQisBusy(app->database_thread->database->database)) {
	//	QMessageBox::warning(this, "Confirmation", "Threads haven't finished processing");
	//	event->ignore();
	//	return;
	//}
	event->accept();
}