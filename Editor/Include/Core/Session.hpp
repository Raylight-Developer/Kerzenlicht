#pragma once

#include "Shared.hpp"

// FWD DECL OTHER
namespace CLASS {
	struct File;
}

// FWD DECL THIS
#define ENDL "\n"

#define ANSI_RESET "\033[0m"

#define ANSI_Black  "\033[90m"
#define ANSI_R      "\033[91m"
#define ANSI_G      "\033[92m"
#define ANSI_Yellow "\033[93m"
#define ANSI_B      "\033[94m"
#define ANSI_Purple "\033[95m"
#define ANSI_Cyan   "\033[96m"
#define ANSI_White  "\033[97m"

// DECL
struct Session {
	static Session& getInstance();

	void flushLog();

	void setLog(Lace* ptr);
	Lace* getLog();

	void setFile(CLASS::File* ptr);
	CLASS::File* getFile();

private:
	Session() : log(nullptr) , file(nullptr) {}
	~Session() = default;

	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	Lace* log;
	CLASS::File* file;
};

#undef LOG
#undef FILE

#define LOG *Session::getInstance().getLog()
#define FILE Session::getInstance().getFile()
#define FLUSH Session::getInstance().flushLog()
#define COUT_S Lace lace; lace <<
#define COUT_E ; cout << lace.str()