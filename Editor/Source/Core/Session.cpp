#include "Core/Session.hpp"

#include "Core/File.hpp"

Session& Session::getInstance() {
	static Session instance;
	return instance;
}

void Session::setLog(Log_Console* ptr) { log = ptr; }

Log_Console* Session::getLog() { return log; }

void Session::setFile(CLASS::File* ptr) { file = ptr; }

CLASS::File* Session::getFile() { return file; }