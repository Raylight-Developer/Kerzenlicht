#pragma once

#include <unordered_map>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename T>
	struct Observable {
		T value;
		unordered_map<void*, function<void(const T&)>> callbacks;

		Observable() : value() {}
		Observable(const T& value) : value(value) {}

		void addCallback(void* key, function<void(const T&)> func) {
			callbacks[key] = std::move(func);
		}

		void removeCallback(void* key) {
			callbacks.erase(key);
		}

		void clearCallbacks() {
			callbacks.clear();
		}

		Observable& operator=(const T& value) {
			this->value = value;
			for (const auto& [key, func] : callbacks) {
				func(value);
			}
			return *this;
		}
	};
}