#pragma once

#include "Include.hpp"

#include "Utils/Inline/Macros.hpp"
#include "Utils//Lace.hpp"

#include "Core/History.hpp"

// FWD DECL OTHER
namespace KL {
	struct File;
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

		void setFile(KL::File* ptr);
		File* getFile();
		File* file;
	};
}