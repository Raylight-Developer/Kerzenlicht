#pragma once

#include <unordered_map>
#include <vector>
#include <map>

#include "../Types.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	template<typename T>
	struct List {
		vector<T> internal_vector;

		List() : internal_vector({}) {}

		void push(const T& value) {
			internal_vector.push_back(value);
		}

		void push_back(const T& value) {
			internal_vector.push_back(value);
		}

		void remove(const T& value) {
			internal_vector.erase(find(internal_vector.begin(), internal_vector.end(), value));
		}

		void remove(const uint64& index) {
			internal_vector.erase(internal_vector.begin(), + index);
		}

		void remove(typename vector<T>::iterator pos) {
			internal_vector.erase(pos);
		}

		void insert(const uint64& index, const T& value) {
			internal_vector.insert(internal_vector.begin() + index, value);
		}

		void extend(typename const vector<T>& other) {
			internal_vector.insert(internal_vector.end(), other.begin(), other.end());
		}

		void extend(typename const List<T>& other) {
			internal_vector.insert(internal_vector.end(), other.internal_vector.begin(), other.internal_vector.end());
		}

		void clear() {
			internal_vector.clear();
		}

		uint64 size() const {
			return internal_vector.size();
		}

		bool empty() const {
			return internal_vector.empty();
		}

		T& operator[](uint64 index) {
			return internal_vector[index];
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