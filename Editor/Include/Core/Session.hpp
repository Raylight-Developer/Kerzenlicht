#pragma once

#include "Include.hpp"

namespace CLASS {
	struct File;
}
struct Log_Console;

struct Session {
	static Session& getInstance();

	void setLog(Log_Console* ptr);
	Log_Console* getLog();

	void setFile(CLASS::File* ptr);
	CLASS::File* getFile();

private:
	Session() : log(nullptr) , file(nullptr) {}
	~Session() = default;

	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	Log_Console* log;
	CLASS::File* file;
};

#undef LOG
#undef FILE

#define LOG Session::getInstance().getLog()
#define FILE Session::getInstance().getFile()
#define FLUSH Session::getInstance().getLog()->flush();
#define COUT_S Lace lace; lace <<
#define COUT_E ; cout << lace.str();