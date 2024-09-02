#include "History.hpp"

void KL::History_Manager::execute(unique_ptr<History_Command> cmd) {
	cmd->execute();
	history_stack.push_back(std::move(cmd));
	call();
}

void KL::History_Manager::undo(const uint64& count) {
	for (uint64_t i = 0; i < count && !history_stack.empty(); ++i) {
		auto cmd = history_stack.back()->invert();
		cmd->execute();
		history_stack.push_back(std::move(cmd));
	}
	call();
}

void KL::History_Manager::redo(const uint64& count) {
	for (uint64_t i = 0; i < count && !history_stack.empty(); ++i) {
		auto cmd = history_stack.back()->invert();
		cmd->execute();
		history_stack.push_back(std::move(cmd));
	}
	call();
}