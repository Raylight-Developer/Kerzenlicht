#include "Utils/Session.hpp"

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

void KL::Session::start() {
	start_time = std::chrono::high_resolution_clock::now();
}

dvec1 KL::Session::f_runtime() {
	const auto end_time = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<dvec1> elapsed_seconds = end_time - start_time;
	return elapsed_seconds.count();
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