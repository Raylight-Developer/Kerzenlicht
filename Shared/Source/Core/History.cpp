#include "Core/History.hpp"

void KL::History_Manager::execute(History_Command* cmd) {
	cmd->execute();
	history_stack.push_back(cmd);
}

void KL::History_Manager::undo(const uint64& count) {
	for (uint64_t i = 0; i < count && !history_stack.empty(); ++i) {
		auto cmd = history_stack.back()->undo();
		cmd->execute();
		history_stack.push_back(cmd);
	}
}

void KL::History_Manager::redo(const uint64& count) {
	for (uint64_t i = 0; i < count && !history_stack.empty(); ++i) {
		auto cmd = history_stack.back()->undo();
		cmd->execute();
		history_stack.push_back(cmd);
	}
}