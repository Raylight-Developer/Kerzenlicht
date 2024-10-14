#include "Core/Utils/Session.hpp"

CORE::Session::Session() :
	log(nullptr),
	history(nullptr)
{}

CORE::Session& CORE::Session::getInstance() {
	static Session instance;
	return instance;
}

void CORE::Session::start() {
	start_time = std::chrono::high_resolution_clock::now();
}

dvec1 CORE::Session::f_runtime() {
	const auto end_time = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<dvec1> elapsed_seconds = end_time - start_time;
	return elapsed_seconds.count();
}

void CORE::Session::setLog(KL::Lace* ptr) { log = ptr; }

KL::Lace* CORE::Session::getLog() { return log; }

void CORE::Session::setHistory(KL::History_Manager* ptr) { history = ptr; }

KL::History_Manager* CORE::Session::getHistory() { return history; }

void CORE::Session::flushLog() {
	cout << log->str();
	log->clear();
}