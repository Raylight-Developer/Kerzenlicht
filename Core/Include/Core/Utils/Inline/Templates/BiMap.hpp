#pragma once

#include <map>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename K, typename V>
	struct BiMap {
		map<K, V> key_to_val;
		map<V, K> val_to_key;

		const V& getVal(const K& key) const {
			return key_to_val.at(key);
		}
		const K& getKey(const V& val) const {
			return val_to_key.at(val);
		}

		void push(const K& key, const V& val) {
			key_to_val[key] = val;
			val_to_key[val] = key;
		}

		void removeKey(const K& key) {
			val_to_key.erase(val_to_key.find(key_to_val.at(key)));
			key_to_val.erase(key_to_val.find(key));
		}
		void removeVal(const V& val) {
			key_to_val.erase(key_to_val.find(val_to_key.at(val)));
			val_to_key.erase(val_to_key.find(val));
		}

		void replace(const K& key, const V& val) {
			key_to_val[key] = val;
			val_to_key[val] = key;
		}

		void clear() {
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