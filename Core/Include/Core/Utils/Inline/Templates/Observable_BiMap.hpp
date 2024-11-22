#pragma once

#include <unordered_map>
#include <map>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename K, typename V>
	struct Observable_BiMap {
		map<K, V> key_to_val;
		map<V, K> val_to_key;

		unordered_map<void*, function<void(const K*, const V*)>> item_added_callbacks;
		unordered_map<void*, function<void(const K*, const V*)>> item_removed_callbacks;
		unordered_map<void*, function<void(const K*, const V*)>> item_replaced_callbacks;

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

		const V& getVal(const K& key) const {
			return key_to_val.at(key);
		}
		const K& getKey(const V& val) const {
			return val_to_key.at(val);
		}

		void push(const K& key, const V& val) {
			for (const auto& [_, func] : item_added_callbacks)
				func(&key, &val);
			key_to_val[key] = val;
			val_to_key[val] = key;
		}

		void removeKey(const K& key) {
			for (const auto& [_, func] : item_removed_callbacks)
				func(&key, &key_to_val[key]);
			val_to_key.erase(val_to_key.find(key_to_val.at(key)));
			key_to_val.erase(key_to_val.find(key));
		}
		void removeVal(const V& val) {
			for (const auto& [_, func] : item_removed_callbacks)
				func(&val_to_key[val], &val);
			key_to_val.erase(key_to_val.find(val_to_key.at(val)));
			val_to_key.erase(val_to_key.find(val));
		}

		void replace(const K& key, const V& val) {
			for (const auto& [_, func] : item_replaced_callbacks)
				func(&key, &val);
			key_to_val[key] = val;
			val_to_key[val] = key;
		}

		void clear() {
			for (auto& [key, val] : key_to_val)
				for (const auto& [_, func] : item_removed_callbacks)
					func(&key, &val);
			key_to_val.clear();
			val_to_key.clear();
		}

		uint64 size() const {
			return key_to_val.size();
		}

		bool empty() const {
			return key_to_val.empty();
		}

		typename vector<K>::const_iterator end() {
			return key_to_val.end();
		}
		typename vector<K>::const_iterator begin() {
			return key_to_val.begin();
		}
		typename vector<K>::const_iterator end() const {
			return key_to_val.end();
		}
		typename vector<K>::const_iterator begin() const {
			return key_to_val.begin();
		}
	};
}