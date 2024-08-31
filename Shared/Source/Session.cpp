#include "Session.hpp"

#ifdef COMPILE_EDITOR
	#include "Core/Editor_File.hpp"
#elif COMPILE_RENDERER
	#include "Core/Render_File.hpp"
#else
	#include "File.hpp"
#endif

KL::Session::Session() :
	log(nullptr),
	file(nullptr)
{}

KL::Session& KL::Session::getInstance() {
	static Session instance;
	return instance;
}

void KL::Session::flushLog() {
	cout << log->str();
	log->clear();
}

void KL::Session::setLog(Lace* ptr) { log = ptr; }

KL::Lace* KL::Session::getLog() { return log; }

#ifdef COMPILE_EDITOR
	void KL::Session::setFile(KL::Editor_File* ptr) { file = ptr; }
	KL::Editor_File* KL::Session::getFile() { return file; }
#elif COMPILE_RENDERER
	void KL::Session::setFile(KL::Render_File* ptr) { file = ptr; }
	KL::Render_File* KL::Session::getFile() { return file; }
#else
	void KL::Session::setFile(KL::File* ptr) { file = ptr; }
	KL::File* KL::Session::getFile() { return file; }
#endif