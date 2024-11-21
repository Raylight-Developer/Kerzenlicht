#pragma once

#include <unordered_map>
#include <map>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename K, typename V>
	struct Observable_Map {
		map<K, V> key_to_val;
		map<V, K> val_to_key;
		unordered_map<void*, function<void(const K*, const V*)>> item_added_callbacks;
		unordered_map<void*, function<void(const K*, const V*)>> item_removed_callbacks;

		Observable_Map() : internal_map({}) {}

		void onItemAddedCallback(void* key, function<void(const K*, const V*)> func) {
			item_added_callbacks[key] = std::move(func);
		}

		void onItemRemovedCallback(void* key, function<void(const K*, const V*)> func) {
			item_removed_callbacks[key] = std::move(func);
		}

		void removeOnItemAddedCallback(void* key) {
			item_added_callbacks.erase(key);
		}

		void removeOnItemRemovedCallback(void* key) {
			item_removed_callbacks.erase(key);
		}

		void clearCallbacks() {
			item_added_callbacks.clear();
			item_removed_callbacks.clear();
		}

		void push(const K& key, const V& val) {
			key_to_val[key] = val;
			val_to_key[val] = key;
			for (const auto& [_, func] : item_added_callbacks)
				func(&key, &val);
		}

		void removeKey(const K& key) {
			auto it = key_to_val.find(key);
			if (it == key_to_val.end()) {
				throw std::runtime_error("Key not found");
			}
			key_to_val.erase(it);
			val_to_key.erase(it->second);
			for (const auto& [_, func] : item_removed_callbacks)
				func(it, it->second);
		}

		void removeVal(const V& val) {
			auto it = val_to_key.find(val);
			if (it == val_to_key.end()) {
				throw std::runtime_error("Value not found");
			}
			val_to_key.erase(it);
			key_to_val.erase(it->second);
			for (const auto& [_, func] : item_removed_callbacks)
				func(it->second, it);
		}

		V& getVal(const K& key) const {
			auto it = key_to_val.find(key);
			if (it != key_to_val.end()) {
				return it->second;
			}
			throw std::runtime_error("Key not found");
		}

		K& getKey(const V& value) const {
			auto it = val_to_key.find(value);
			if (it != val_to_key.end()) {
				return it->second;
			}
			throw std::runtime_error("Value not found");
		}

		void clear() {
			for (auto& [key, val] : internal_map)
				for (const auto& [_, func] : item_removed_callbacks)
					func(&key, &val);
			internal_map.clear();
		}

		uint64 size() const {
			return internal_map.size();
		}

		bool empty() const {
			return internal_map.empty();
		}

		typename vector<K>::const_iterator end() {
			return internal_map.end();
		}
		typename vector<K>::const_iterator begin() {
			return internal_map.begin();
		}
		typename vector<K>::const_iterator end() const {
			return internal_map.end();
		}
		typename vector<K>::const_iterator begin() const {
			return internal_map.begin();
		}
	};
}