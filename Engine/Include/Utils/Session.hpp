#pragma once

#include "Core.hpp"

#include "Core/File.hpp"

// FWD DECL OTHER
namespace KL {
	struct File;
	struct Lace;
}

// FWD DECL THIS

// DECL
namespace KL {
	struct Session : CORE::Session {
		File* file;
		static Session& getInstance();

		Session();
		~Session() = default;
		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;

		void setFile(KL::File* ptr);
		File* getFile();
	};
}

#undef TIMER_END
#undef SESSION
#undef CMD
#undef UNDO
#undef REDO
#undef LOG
#undef FLUSH

#define TIMER_END auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> deltaTime = end - start; *KL::Session::getInstance().getLog() <<  KL::Lace_NL() << "Delta time: " << deltaTime.count() << " seconds"; FLUSH
#define SESSION(prop) KL::Session::getInstance().properties[prop]
#define CMD(command)  KL::Session::getInstance().getHistory()->execute(command)
#define UNDO(count)   KL::Session::getInstance().getHistory()->undo(count)
#define REDO(count)   KL::Session::getInstance().getHistory()->redo(count)
#define LOG          *KL::Session::getInstance().getLog()
#define FLUSH         KL::Session::getInstance().flushLog()

#if defined(COMPILE_EDITOR) || defined(COMPILE_GUI_SCRIPTING)
	#define FILE static_cast<KL::Editor_File*>(KL::Session::getInstance().getFile())
#elif defined(COMPILE_RENDERER)
	#define FILE static_cast<KL::Render_File*>(KL::Session::getInstance().getFile())
#else
	#define FILE KL::Session::getInstance().getFile()
#endif