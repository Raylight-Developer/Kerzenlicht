#pragma once

#include "Include.hpp"

#include "Lace.hpp"
#include "Ops.hpp"

namespace KL {
	struct History_Command {
		Lace info;
		Lace details;
		virtual ~History_Command() = default;
		virtual void execute() = 0;
		virtual unique_ptr<History_Command> invert() const = 0;
	};

	struct History_Manager : Observable {
		vector<unique_ptr<History_Command>> history_stack;

		void execute(unique_ptr<History_Command> cmd);
		void undo(const uint64& count);
		void redo(const uint64& count);
	};
}