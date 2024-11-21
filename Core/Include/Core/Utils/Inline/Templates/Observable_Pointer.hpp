#pragma once

#include <unordered_map>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename T>
	struct Observable_Ptr {
		T* pointer;
		unordered_map<void*, function<void()>> callbacks;

		Observable_Ptr() : pointer(nullptr) {}
		Observable_Ptr(T* pointer) : pointer(pointer) {}
		~Observable_Ptr() {
			delete pointer;
		}

		void addCallback(void* key, function<void()> func) {
			callbacks[key] = std::move(func);
		}

		void removeCallback(void* key) {
			callbacks.erase(key);
		}

		void clearCallbacks() {
			callbacks.clear();
		}

		Observable_Ptr& operator=(T* pointer) {
			this->pointer = pointer;
			for (const auto& [key, func] : callbacks) {
				func();
			}
			return *this;
		}
	};
}