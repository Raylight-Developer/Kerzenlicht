#include "Core/Session.hpp"

#include "Core/File.hpp"

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

void Session::setFile(CLASS::File* ptr) { file = ptr; }

CLASS::File* Session::getFile() { return file; }