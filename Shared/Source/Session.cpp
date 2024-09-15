#include "Session.hpp"

#include "File.hpp"

KL::Session::Session() :
	log(nullptr),
	uid(nullptr),
	file(nullptr),
	history(nullptr)
{}

KL::Session& KL::Session::getInstance() {
	static Session instance;
	return instance;
}

void KL::Session::setUID(uint64* ptr) { uid = ptr; }

uint64* KL::Session::getUID() { return uid; }

void KL::Session::setLog(Lace* ptr) { log = ptr; }

KL::Lace* KL::Session::getLog() { return log; }

void KL::Session::setHistory(History_Manager* ptr) { history = ptr; }

KL::History_Manager* KL::Session::getHistory() { return history; }

void KL::Session::flushLog() {
	cout << log->str();
	log->clear();
}

void KL::Session::setFile(KL::File* ptr) { file = ptr; }
KL::File* KL::Session::getFile() { return file; }