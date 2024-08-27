#pragma once

#include "Include.hpp"
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
		uint64 ram = 0;
		uint64 vram = 0;
		//vector<tuple<string, unordered_map<uint64, uint64>, Data>> history;

		static Session& getInstance();

		Session();
		~Session() = default;
		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;

		void setLog(KL::Lace* ptr);
		KL::Lace* getLog();

		void flushLog();

		#ifdef COMPILE_EDITOR
			void setFile(KL::Editor_File* uptr);
			KL::Editor_File* getFile();
			KL::Editor_File* file;
		#elif COMPILE_RENDERER
			void setFile(KL::Render_File* ptr);
			KL::Render_File* getFile();
			KL::Render_File* file;
		#else
			void setFile(KL::File* ptr);
			KL::File* getFile();
			KL::File* file;
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

#define RAM KL::Session::getInstance().ram
#define RAM_A(type, count) KL::Session::getInstance().ram += (sizeof(type) * count)
#define RAM_R(type, count) KL::Session::getInstance().ram -= (sizeof(type) * count)

#define VRAM KL::Session::getInstance().vram
#define VRAM_A(type, count) KL::Session::getInstance().vram += (sizeof(type) * count)
#define VRAM_R(type, count) KL::Session::getInstance().vram -= (sizeof(type) * count)

#define LOG *KL::Session::getInstance().getLog()
#define FILE KL::Session::getInstance().file
#define FLUSH KL::Session::getInstance().flushLog()
#define COUT_S KL::Lace lace; lace <<
#define COUT_E ; cout << lace.str()