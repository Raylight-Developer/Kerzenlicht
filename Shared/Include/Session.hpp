#pragma once

#include "Include.hpp"
#include "History.hpp"
#include "Macros.hpp"
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