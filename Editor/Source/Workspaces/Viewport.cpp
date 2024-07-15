#include "Workspaces/Viewport.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Timeline::Timeline(Workspace_Viewport* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::LeftToRight),
	parent(parent)
{
	slider = new GUI::Slider(this);
	slider->setRange(0, 240);

	info = new GUI::Label(this);
	info->setText("FPS: 0 Frame: ");
	info->setFixedWidth(64);

	GUI::Value_Input* current_frame = new GUI::Value_Input(this);
	current_frame->setText("0");
	QIntValidator* validator = new QIntValidator(slider->minimum(), slider->maximum(), current_frame);
	current_frame->setValidator(validator);

	connect(slider, &GUI::Slider::valueChanged, [this, current_frame](int value) {
		current_frame->setText(QString::number(value));
		FILE->active_scene->ptr->current_frame = value;

		//if (this->parent->viewport_gpu_renderer) this->parent->viewport_gpu_renderer->f_updateFrame();
		//if (this->parent->viewport_realtime) this->parent->viewport_realtime->f_updateFrame();
	});
	connect(current_frame, &GUI::Value_Input::returnPressed, [this, current_frame]() {
		slider->setValue(current_frame->text().toInt());
	});
	connect(current_frame, &GUI::Value_Input::textChanged, [this, current_frame](const QString& text) {
		if (text.toDouble() > slider->maximum())
			current_frame->setText(QString::number(slider->maximum()));
		else if (text.toDouble() < slider->minimum())
			current_frame->setText(QString::number(slider->minimum()));
	});

	addWidget(slider);
	addWidget(current_frame);
	addWidget(info);
}

GUI::WORKSPACE::Workspace_Viewport::Workspace_Viewport(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	f_systemInfo();

	viewport_realtime = new Viewport_Realtime(this);
	viewport_realtime->init();
	QWindow *qWindow = QWindow::fromWinId((WId)glfwGetWin32Window(viewport_realtime->window));
	qWindow->setSurfaceType(QSurface::OpenGLSurface);
	container = QWidget::createWindowContainer(qWindow);

	container->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	container->setContentsMargins(0, 0, 0, 0);

	timeline = new Timeline(this);

	GUI::Splitter* splitter = new GUI::Splitter(this, true);
	splitter->addWidget(container);
	splitter->addWidget(timeline);
	addWidget(splitter);
}

GUI::WORKSPACE::Workspace_Viewport::~Workspace_Viewport() {
}

void GUI::WORKSPACE::Workspace_Viewport::f_systemInfo() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	*LOG
	<< ENDL << "┌"
	<< ENDL << "│  System"
	<< ENDL << "│    Total physical RAM: " << uint(ceil((double)(status.ullTotalPhys / (1024.0 * 1024.0 * 1024.0)))) << "GB - " << status.ullTotalPhys / (1024 * 1024) << "MB"
	<< ENDL << "│  CPU"
	<< ENDL << "│    Threads: " << thread::hardware_concurrency()
	<< ENDL << "│  GPU"
	<< ENDL << "└";
}