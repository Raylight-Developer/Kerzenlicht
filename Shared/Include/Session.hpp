#pragma once

#include "Include.hpp"
#include "History.hpp"
#include "Lace.hpp"

// FWD DECL OTHER
namespace KL {
	#ifdef COMPILE_EDITOR
		struct Editor_File;
	#elif COMPILE_RENDERER
		struct Render_File;
	#else
		struct File;
	#endif
	struct Lace;
}

// FWD DECL THIS

// DECL
namespace KL {
	struct Session {
		Lace* log;
		uint64* uid;
		uint64 ram = 0;
		uint64 vram = 0;
		History_Manager* history;

		static Session& getInstance();

		Session();
		~Session() = default;
		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;

		void setUID(uint64* ptr);
		uint64* getUID();

		void setLog(Lace* ptr);
		Lace* getLog();

		void setHistory(History_Manager* ptr);
		History_Manager* getHistory();

		void flushLog();

		#ifdef COMPILE_EDITOR
			void setFile(KL::Editor_File* pointer);
			Editor_File* getFile();
			Editor_File* file;
		#elif COMPILE_RENDERER
			void setFile(KL::Render_File* ptr);
			Render_File* getFile();
			Render_File* file;
		#else
			void setFile(KL::File* ptr);
			File* getFile();
			File* file;
		#endif
	};
}

#undef LOG
#undef FILE

#define ENDL << KL::Lace_NL()

#define ANSI_RESET  << "\033[0m"

#define ANSI_BLACK  << "\033[90m"
#define ANSI_R      << "\033[91m"
#define ANSI_G      << "\033[92m"
#define ANSI_YELLOW << "\033[93m"
#define ANSI_B      << "\033[94m"
#define ANSI_PURPLE << "\033[95m"
#define ANSI_CYAN   << "\033[96m"
#define ANSI_WHITE  << "\033[97m"

#define NL  << KL::Lace_NL()
#define SP  << KL::Lace_S()
#define TAB << KL::Lace_TAB()
#define PTR << "* "

#define CMD(command) KL::Session::getInstance().getHistory()->execute(command);
#define UNDO(count)  KL::Session::getInstance().getHistory()->undo(count);
#define REDO(count)  KL::Session::getInstance().getHistory()->redo(count);

#define NEW_UID KL::Session::getInstance().getUID()++

#define RAM KL::Session::getInstance().ram
#define RAM_A(type, count) KL::Session::getInstance().ram += (sizeof(type) * count)
#define RAM_R(type, count) KL::Session::getInstance().ram -= (sizeof(type) * count)

#define VRAM KL::Session::getInstance().vram
#define VRAM_A(type, count) KL::Session::getInstance().vram += (sizeof(type) * count)
#define VRAM_R(type, count) KL::Session::getInstance().vram -= (sizeof(type) * count)

#define LOG *KL::Session::getInstance().getLog()
#define FILE KL::Session::getInstance().getFile()
#define FLUSH KL::Session::getInstance().flushLog()
#define COUT_S KL::Lace lace; lace <<
#define COUT_E ; cout << lace.str()