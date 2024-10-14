#include "Utils/Session.hpp"

KL::Session::Session() :
	CORE::Session(),
	file(nullptr)
{}

KL::Session& KL::Session::getInstance() {
	static Session instance;
	return instance;
}

void KL::Session::setFile(KL::File* ptr) { file = ptr; }
KL::File* KL::Session::getFile() { return file; }