#include "Workspaces/API.hpp"

#include "Workspaces/Manager.hpp"

#undef slots
#include "Core/Python_API.hpp"

GUI::WORKSPACE::Workspace_API::Workspace_API(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	GUI::Button* Execute_Button = new GUI::Button(this);
	Execute_Button->setText("Execute");
	GUI::Button* Edit_Button = new GUI::Button(this);
	Edit_Button->setText("Edit File");

	GUI::Linear_Contents* Header = new GUI::Linear_Contents(this, QBoxLayout::Direction::LeftToRight);
	Header->addWidget(Execute_Button);
	Header->addWidget(Edit_Button);
	Header->setMaximumHeight(Execute_Button->sizeHint().height());

	console_output = new GUI::Text_Stream(this);
	console_output_history = new GUI::Text_Stream(this);
	history_counter = 0;

	GUI::Splitter* Out_Splitter = new GUI::Splitter(this, false);
	Out_Splitter->addWidget(console_output);
	Out_Splitter->addWidget(console_output_history);

	GUI::Splitter* VSplitter = new GUI::Splitter(this, true);
	VSplitter->addWidget(Header);
	VSplitter->addWidget(Out_Splitter);

	addWidget(VSplitter);

	connect(Execute_Button, &GUI::Button::clicked, this, &Workspace_API::f_executeCode);
	connect(Edit_Button, &GUI::Button::clicked, this, &Workspace_API::f_editCode);
}

void GUI::WORKSPACE::Workspace_API::f_editCode() {
	Py::scoped_interpreter guard{};
	try {
		Py::module sys = Py::module::import("sys");
		Py::module os = Py::module::import("os");
	
		Py::object _stdout = sys.attr("stdout");
		Py::object _stderr = sys.attr("stderr");
		auto stringio = Py::module::import("io").attr("StringIO");
		Py::object _stdout_buffer = stringio();
		Py::object _stderr_buffer = stringio();
		sys.attr("stdout") = _stdout_buffer;
		sys.attr("stderr") = _stderr_buffer;
	
		Py::exec("os.startfile('D:/Kerzenlicht Renderer/Engine/Resources/Python/Runtime.py')");
	}
	catch (...) {
	}
}

void GUI::WORKSPACE::Workspace_API::f_executeCode() {
	Py::scoped_interpreter guard{};
	try {
		Py::module sys = Py::module::import("sys");
		Py::module os = Py::module::import("os");
		Py::module Kerz = Py::module::import("KL");
	
		Py::dict Globals = Py::globals();
		Globals["window"] = Py::cast(parent->parent);
	
		Py::object _stdout = sys.attr("stdout");
		Py::object _stderr = sys.attr("stderr");
		auto stringio = Py::module::import("io").attr("StringIO");
		Py::object _stdout_buffer = stringio();
		Py::object _stderr_buffer = stringio();
		sys.attr("stdout") = _stdout_buffer;
		sys.attr("stderr") = _stderr_buffer;
	
		Py::exec("exec(open('D:/Kerzenlicht Renderer/Engine/Resources/Python/Runtime.py', 'r').read())");
	
		_stdout_buffer.attr("seek")(0);
		_stderr_buffer.attr("seek")(0);
	
		string Out = Py::str(_stdout_buffer.attr("read")());
		string Err = Py::str(_stderr_buffer.attr("read")());
	
		if (Err.size() == 0) {
			console_output->setText(QString::fromStdString(Out));
			console_output_history->setText(console_output_history->toPlainText() + "╠══[" + QString::fromStdString(to_string(history_counter)) + "]══════════╣\n" + QString::fromStdString(Out + "\n"));
		}
		else {
			console_output->setText(QString::fromStdString(Out + "!! Error !!\n" + Err));
			console_output_history->setText(console_output_history->toPlainText() + "╠══[" + QString::fromStdString(to_string(history_counter)) + "]══════════╣\n" + QString::fromStdString(Out + "!! Error !!\n" + Err + "\n"));
		}
	}
	catch (const exception& e) {
		Lace Err;
		Err << "Error occurred: " << e.what();
		console_output->setText(QString::fromStdString(Err.str()));
		console_output_history->setText(console_output_history->toPlainText() + "╠══[" + QString::fromStdString(to_string(history_counter)) + "]══════════╣\n" + QString::fromStdString(Err.str() + "\n"));
	}
	history_counter++;
}

void GUI::WORKSPACE::Workspace_API::keyPressEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key_F5) f_executeCode();
	else Linear_Contents::keyPressEvent(event);
}