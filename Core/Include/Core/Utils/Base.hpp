#pragma once

#include <unordered_map>
#include <functional>

#include "Inline/Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
struct Base {
	string ID;
	Base* parent;
	vector<Base*> children;
	unordered_map<void*, function<void(Base*, const uint&)>> callbacks;

	Base(Base* parent = nullptr) :
		parent(parent)
	{
		if (parent) {
			parent->children.push_back(this);
		}
	}
	~Base() {
		if (parent) {
			parent->children.erase(find(parent->children.begin(), parent->children.end(),this));
		}
	}

	void setParent(Base* new_parent) {
		if (parent) {
			parent->children.erase(find(parent->children.begin(), parent->children.end(),this));
		}
		parent = new_parent;
		if (parent) {
			parent->children.push_back(this);
		}
	}

	// On Child Called
	virtual void onChildCall(Base* child, const uint& instruction) {
		childCall(child, instruction);
	}

	// On Self Called
	virtual void onCall() {
		cout << ID << endl;
	}

	void call(const uint& instruction = 0) {
		for (const auto& [key, func] : callbacks) {
			func(this, instruction);
		}
		onCall();
	}

	void callUpstream(const uint& instruction = 0) {
		if (parent) {
			parent->onChildCall(this, instruction);
		}
	}

	void fullCall(const uint& instruction = 0) {
		call(instruction);
		callUpstream(instruction);
	}

	void childCall(Base* child, const uint& instruction) const {
		Base* node = child;
		vector<string> IDs;
		while (node and node != parent) {
			IDs.push_back(node->ID);
			node = node->parent;
		}
		reverse(IDs.begin(), IDs.end());
		for (uint64 i = 0; i < IDs.size(); i++) {
			cout << IDs[i];
			if (i < IDs.size() - 1) {
				cout << ".";
			}
		}
		cout << endl;
	}

	void addCallback(void* key, function<void(Base*, const uint&)> func) {
		callbacks[key] = func;
	}
	void removeCallback(void* key) {
		callbacks.erase(key);
	}
	void clearCallbacks() {
		callbacks.clear();
	}
};