#pragma once

#include "Include.hpp"

#include "Data_Property.hpp"
#include "Session.hpp"

#include "Ops.hpp"

namespace KL {
	struct History_Command {
		virtual ~History_Command() = default;
		virtual void execute() = 0;
		virtual History_Command* invert() = 0;
	};

	struct History_Manager : Observable {
		stack<History_Command*> history_stack;

		void execute(History_Command* cmd);
		void undo(const uint64& count);
		void redo(const uint64& count);
	};
}