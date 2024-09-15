#include "Workspaces/Viewport.hpp"

#include "Workspaces/Manager.hpp"

//#define PATH_TRACING true

GUI::WORKSPACE::Timeline::Timeline(Workspace_Viewport* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::LeftToRight),
	parent(parent)
{
	slider = new GUI::Slider(this);
	slider->setRange(0, 360);

	info = new GUI::Label(this);
	info->setText("FPS: 0 | Frame: 0 | Delta: 0.016ms");

	GUI::Value_Input* current_frame = new GUI::Value_Input(this);
	current_frame->setText("0");
	QIntValidator* validator = new QIntValidator(slider->minimum(), slider->maximum(), current_frame);
	current_frame->setValidator(validator);

	connect(slider, &GUI::Slider::valueChanged, [this, current_frame](int value) {
		current_frame->setText(QString::number(value));
		FILE->active_scene->pointer->current_frame = value;

		//if (this->parent->viewport) this->parent->viewport->f_tickUpdate();
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

	addWidget(info);
	addWidget(current_frame);
	addWidget(slider);
}

GUI::WORKSPACE::Workspace_Viewport::Workspace_Viewport(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	f_systemInfo();

	viewport = new Viewport(this);
	timeline = new Timeline(this);

	GUI::Splitter* splitter = new GUI::Splitter(this, true);
	splitter->addWidget(viewport);
	splitter->addWidget(timeline);
	addWidget(splitter);
}

GUI::WORKSPACE::Workspace_Viewport::~Workspace_Viewport() {
}

void GUI::WORKSPACE::Workspace_Viewport::f_systemInfo() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	LOG ENDL ANSI_B << "[System]" ANSI_RESET;
	LOG ENDL ANSI_B << "  [RAM]" ANSI_RESET;
	LOG ENDL << "    " << d_to_u((status.ullTotalPhys / (1024 * 1024 * 1024))) << " GB - " << status.ullTotalPhys / (1024 * 1024) << " MB";
	LOG ENDL ANSI_B << "  [CPU]" ANSI_RESET;
	LOG ENDL << "    " << thread::hardware_concurrency() << " Threads";
	LOG ENDL ANSI_B << "  [GPU]" ANSI_RESET;
	FLUSH;
}

GUI::WORKSPACE::Viewport::Viewport(Workspace_Viewport* parent) :
	parent(parent)
{
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	setContentsMargins(0, 0, 0, 0);
}

GUI::WORKSPACE::Render_Thread::Render_Thread() {

}

void GUI::WORKSPACE::Render_Thread::run() {

}