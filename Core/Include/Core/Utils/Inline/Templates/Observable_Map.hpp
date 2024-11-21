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
		map<K, V> internal_map;
		unordered_map<void*, function<void(const K*, const V*)>> item_added_callbacks;
		unordered_map<void*, function<void(const K*, const V*)>> item_removed_callbacks;
		unordered_map<void*, function<void(const K*, const V*)>> item_replaced_callbacks;

		Observable_Map() : internal_map({}) {}

		void onItemAddedCallback(void* key, function<void(const K*, const V*)> func) {
			item_added_callbacks[key] = std::move(func);
		}
		void onItemRemovedCallback(void* key, function<void(const K*, const V*)> func) {
			item_removed_callbacks[key] = std::move(func);
		}
		void onItemReplacedCallback(void* key, function<void(const K*, const V*)> func) {
			item_replaced_callbacks[key] = std::move(func);
		}

		void removeOnItemAddedCallback(void* key) {
			item_added_callbacks.erase(key);
		}
		void removeOnItemRemovedCallback(void* key) {
			item_removed_callbacks.erase(key);
		}
		void removeOnItemReplacedCallback(void* key) {
			item_replaced_callbacks.erase(key);
		}

		void clearCallbacks() {
			item_added_callbacks.clear();
			item_removed_callbacks.clear();
			item_replaced_callbacks.clear();
		}

		V& at(const K& key) const {
			return internal_map[key];
		}

		const V& operator[](const K& key) const {
			return internal_map.at(key);
		}

		void push(const K& key, const V& val) {
			for (const auto& [_, func] : item_added_callbacks)
				func(&key, &val);
			internal_map[key] = val;
		}

		void remove(const K& key) {
			for (const auto& [_, func] : item_removed_callbacks)
				func(&key, &internal_map[key]);
			internal_map.erase(internal_map.find(key));
		}

		void replace(const K& key, const V& val) {
			for (const auto& [_, func] : item_updated_callbacks)
				func(&key, &val);
			internal_map[key] = val;
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