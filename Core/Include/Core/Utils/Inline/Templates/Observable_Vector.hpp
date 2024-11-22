#pragma once

#include <unordered_map>
#include <vector>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename T>
	struct Observable_List {
		vector<T> internal_vector;
		unordered_map<void*, function<void(const T*)>> item_added_callbacks;
		unordered_map<void*, function<void(const T*)>> item_removed_callbacks;
		unordered_map<void*, function<void(const T*)>> item_replaced_callbacks;

		void onItemAddedCallback(void* key, function<void(const T*)> func) {
			item_added_callbacks[key] = std::move(func);
		}
		void onItemRemovedCallback(void* key, function<void(const T*)> func) {
			item_removed_callbacks[key] = std::move(func);
		}
		void onItemReplacedCallback(void* key, function<void(const T*)> func) {
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

		T& at(const uint64& index) const {
			return internal_vector[index];
		}

		const T& operator[](const uint64& index) const {
			return internal_vector.at(index);
		}

		void push(const T& value) {
			for (const auto& [key, func] : item_added_callbacks)
				func(&value);
			internal_vector.push_back(value);
		}

		void remove(const T& value) {
			for (const auto& [key, func] : item_removed_callbacks)
				func(&value);
			internal_vector.erase(find(internal_vector.begin(), internal_vector.end(), value));
		}

		void remove(const uint64& index) {
			for (const auto& [key, func] : item_removed_callbacks)
				func(&internal_vector[index]);
			internal_vector.erase(internal_vector.begin(), + index);
		}

		void replace(const uint64& index, const T& value) {
			for (const auto& [key, func] : item_replaced_callbacks)
				func(index, &value);
			internal_vector[index] = value;
		}

		void insert(const uint64& index, const T& value) {
			for (const auto& [key, func] : item_added_callbacks)
				func(&value);
			internal_vector.insert(internal_vector.begin() + index, value);
		}

		void extend(typename const vector<T>& other) {
			internal_vector.reserve(other.size());
			for (auto& item : other) {
				internal_vector.push_back(item);
				for (const auto& [key, func] : item_added_callbacks)
					func(&item);
			}
		}

		void extend(typename const List<T>& other) {
			internal_vector.reserve(other.size());
			for (auto& item : other) {
				internal_vector.push_back(item);
				for (const auto& [key, func] : item_added_callbacks)
					func(&item);
			}
		}

		void extend(typename const Observable_List<T>& other) {
			internal_vector.reserve(other.size());
			for (auto& item : other) {
				internal_vector.push_back(item);
				for (const auto& [key, func] : item_added_callbacks)
					func(&item);
			}
		}

		void clear() {
			for (T& item : internal_vector)
				for (const auto& [key, func] : item_removed_callbacks)
					func(item);
			internal_vector.clear();
		}

		uint64 size() const {
			return internal_vector.size();
		}

		bool empty() const {
			return internal_vector.empty();
		}

		T back() const {
			return internal_vector.back();
		}
		T& back() {
			return internal_vector.back();
		}
		typename vector<T>::const_iterator end() {
			return internal_vector.end();
		}
		typename vector<T>::const_iterator begin() {
			return internal_vector.begin();
		}
		typename vector<T>::const_iterator end() const {
			return internal_vector.end();
		}
		typename vector<T>::const_iterator begin() const {
			return internal_vector.begin();
		}
	};
}