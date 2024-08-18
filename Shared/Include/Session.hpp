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

// DECL
namespace KL {
	struct Session {
		static Session& getInstance();

		void flushLog();

		void setLog(Lace* uptr);
		Lace* getLog();

		#ifdef COMPILE_EDITOR
			void setFile(KL::Editor_File* uptr);
			KL::Editor_File* getFile();
		#elif COMPILE_RENDERER
			void setFile(KL::Render_File* ptr);
			KL::Render_File* getFile();
		#else
			void setFile(KL::File* ptr);
			KL::File* getFile();
		#endif

	private:
		Session();
		~Session() = default;

		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;

		Lace* log;
		#ifdef COMPILE_EDITOR
			KL::Editor_File* file;
		#elif COMPILE_RENDERER
			KL::Render_File* file;
		#else
			KL::File* file;
		#endif
	};
}

#undef LOG
#undef FILE

#define LOG *KL::Session::getInstance().getLog()
#define FILE KL::Session::getInstance().getFile()
#define FLUSH KL::Session::getInstance().flushLog()
#define COUT_S KL::Lace lace; lace <<
#define COUT_E ; cout << lace.str()