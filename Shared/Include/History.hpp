#pragma once

#include "Include.hpp"

#include "Lace.hpp"
#include "Ops.hpp"

namespace KL {
	namespace HISTORY {
		enum struct Type {
			NONE,
			SHADER
		};
	}
	struct History_Command {
		Lace info;
		Lace details;
		uint16 sub_type;
		HISTORY::Type type;
		virtual ~History_Command() = default;
		virtual void execute() = 0;
		virtual unique_ptr<History_Command> undo() const = 0;
		virtual void serialize(Lace& lace) const = 0;
		virtual void deserialize(const string& value) = 0;
	};

	struct History_Manager : Observable {
		vector<unique_ptr<History_Command>> history_stack;

		void execute(unique_ptr<History_Command> cmd);
		void undo(const uint64& count);
		void redo(const uint64& count);
	};
}