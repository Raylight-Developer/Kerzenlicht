#pragma once

#include "Core/Include.hpp"

#include "Core/Utils/Inline/Macros.hpp"
#include "Core/Utils//Lace.hpp"

#include "Core/Core/History.hpp"
#include "Core/Core/Property.hpp"

// FWD DECL OTHER
namespace KL {
	struct Lace;
}

// FWD DECL THIS

// DECL
namespace CORE {
	struct Session {
		KL::Lace* log;
		KL::History_Manager* history;
		unordered_map<string, CORE::Prop> properties;
		std::chrono::high_resolution_clock::time_point start_time;

		static Session& getInstance();

		Session();
		~Session() = default;
		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;

		void start();
		dvec1 f_runtime();

		void setLog(KL::Lace* ptr);
		KL::Lace* getLog();

		void setHistory(KL::History_Manager* ptr);
		KL::History_Manager* getHistory();

		void flushLog();
	};
}