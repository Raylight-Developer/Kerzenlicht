#pragma once

#include <unordered_map>
#include <map>

#include "../Types.hpp"
// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template <typename K, typename V>
	struct BiMap {
		map<K, V> key_to_val;
		map<V, K> val_to_key;

		void push(const K& key, const V& val) {
			if (key_to_val.find(key) != key_to_val.end() || val_to_key.find(val) != val_to_key.end()) {
				throw std::runtime_error("Key or value already exists");
			}
			key_to_val[key] = val;
			val_to_key[val] = key;
		}

		void removeKey(const K& key) {
			auto it = key_to_val.find(key);
			if (it == key_to_val.end()) {
				throw std::runtime_error("Key not found");
			}
			V value = it->second;
			key_to_val.erase(it);
			val_to_key.erase(value);
		}

		void removeVal(const V& val) {
			auto it = val_to_key.find(val);
			if (it == val_to_key.end()) {
				throw std::runtime_error("Value not found");
			}
			K key = it->second;
			val_to_key.erase(it);
			key_to_val.erase(key);
		}

		V getVal(const K& key) const {
			auto it = key_to_val.find(key);
			if (it != key_to_val.end()) {
				return it->second;
			}
			throw std::runtime_error("Key not found");
		}

		K getKey(const V& value) const {
			auto it = val_to_key.find(value);
			if (it != val_to_key.end()) {
				return it->second;
			}
			throw std::runtime_error("Value not found");
		}

		void clear() {
			key_to_val.clear();
			val_to_key.clear();
		}

		uint64 size() const {
			return key_to_val.size();
		}
	};

	template <typename K, typename V>
	struct Unordered_BiMap {
		unordered_map<K, V> key_to_val;
		unordered_map<V, K> val_to_key;

		void push(const K& key, const V& val) {
			if (key_to_val.find(key) != key_to_val.end() || val_to_key.find(val) != val_to_key.end()) {
				throw std::runtime_error("Key or value already exists");
			}
			key_to_val[key] = val;
			val_to_key[val] = key;
		}

		void removeKey(const K& key) {
			auto it = key_to_val.find(key);
			if (it == key_to_val.end()) {
				throw std::runtime_error("Key not found");
			}
			V value = it->second;
			key_to_val.erase(it);
			val_to_key.erase(value);
		}

		void removeVal(const V& val) {
			auto it = val_to_key.find(val);
			if (it == val_to_key.end()) {
				throw std::runtime_error("Value not found");
			}
			K key = it->second;
			val_to_key.erase(it);
			key_to_val.erase(key);
		}

		V getVal(const K& key) const {
			auto it = key_to_val.find(key);
			if (it != key_to_val.end()) {
				return it->second;
			}
			throw std::runtime_error("Key not found");
		}

		K getKey(const V& value) const {
			auto it = val_to_key.find(value);
			if (it != val_to_key.end()) {
				return it->second;
			}
			throw std::runtime_error("Value not found");
		}

		void clear() {
			key_to_val.clear();
			val_to_key.clear();
		}

		uint64 size() const {
			return key_to_val.size();
		}
	};
}