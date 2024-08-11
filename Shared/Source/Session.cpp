#include "Session.hpp"

#include "File.hpp"
#include "Lace.hpp"

Session& Session::getInstance() {
	static Session instance;
	return instance;
}

void Session::flushLog() {
	cout << log->str();
	log->clear();
}

void Session::setLog(Lace* ptr) { log = ptr; }

Lace* Session::getLog() { return log; }

#ifdef COMPILE_EDITOR
void Session::setFile(CLASS::Editor_File* ptr) { file = ptr; }

CLASS::Editor_File* Session::getFile() { return file; }
#elif COMPILE_RENDERER
void Session::setFile(CLASS::Render_File* ptr) { file = ptr; }

CLASS::Render_File* Session::getFile() { return file; }
#else
void Session::setFile(CLASS::File* ptr) { file = ptr; }

CLASS::File* Session::getFile() { return file; }
#endif