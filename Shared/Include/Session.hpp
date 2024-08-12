#pragma once

#include "Include.hpp"

#include "Lace.hpp"

// FWD DECL OTHER
namespace CLASS {
	#ifdef COMPILE_EDITOR
		struct Editor_File;
	#elif COMPILE_RENDERER
		struct Render_File;
	#else
		struct File;
	#endif
}

// FWD DECL THIS
#define ENDL Lace_NL()

#define ANSI_RESET "\033[0m"

#define ANSI_Black  "\033[90m"
#define ANSI_R      "\033[91m"
#define ANSI_G      "\033[92m"
#define ANSI_Yellow "\033[93m"
#define ANSI_B      "\033[94m"
#define ANSI_PURPLE "\033[95m"
#define ANSI_Cyan   "\033[96m"
#define ANSI_White  "\033[97m"

// DECL
struct Session {
	static Session& getInstance();

	void flushLog();

	void setLog(Lace* ptr);
	Lace* getLog();

	#ifdef COMPILE_EDITOR
		void setFile(CLASS::Editor_File* ptr);
		CLASS::Editor_File* getFile();
	#elif COMPILE_RENDERER
		void setFile(CLASS::Render_File* ptr);
		CLASS::Render_File* getFile();
	#else
		void setFile(CLASS::File* ptr);
		CLASS::File* getFile();
	#endif

private:
	Session() : log(nullptr) , file(nullptr) {}
	~Session() = default;

	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	Lace* log;
	#ifdef COMPILE_EDITOR
		CLASS::Editor_File* file;
	#elif COMPILE_RENDERER
		CLASS::Render_File* file;
	#else
		CLASS::File* file;
	#endif
};

#undef LOG
#undef FILE

#define LOG *Session::getInstance().getLog()
#define FILE Session::getInstance().getFile()
#define FLUSH Session::getInstance().flushLog()
#define COUT_S Lace lace; lace <<
#define COUT_E ; cout << lace.str()