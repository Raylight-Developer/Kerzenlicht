#pragma once

#include <unordered_map>
#include <vector>
#include <map>

#include "Types.hpp"

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

		void set(T* pointer) {
			this->pointer = pointer;
			for (const auto& [key, func] : callbacks)
				func();
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
	};

	enum struct Observable_Vector_Operation {
		ADD,
		REMOVE,
		CLEAR
	};

	template<typename T>
	struct Observable_Vector {
		vector<T> internal_vector;
		unordered_map<void*, function<void(Observable_Vector_Operation)>> callbacks;

		Observable_Vector() : internal_vector({}) {}

		void addCallback(void* key, function<void(Observable_Vector_Operation)> func) {
			callbacks[key] = std::move(func);
		}

		void removeCallback(void* key) {
			callbacks.erase(key);
		}

		void clearCallbacks() {
			callbacks.clear();
		}

		void notify(const Observable_Vector_Operation& type) {
			for (const auto& [key, func] : callbacks)
				func(type);
		}

		void push_back(const T& value) {
			internal_vector.push_back(value);
			notify(Observable_Vector_Operation::ADD);
		}

		void erase(typename vector<T>::iterator pos) {
			internal_vector.erase(pos);
			notify(Observable_Vector_Operation::REMOVE);
		}

		void clear() {
			internal_vector.clear();
			notify(Observable_Vector_Operation::CLEAR);
		}

		T& operator[](uint64 index) {
			return internal_vector[index];
		}

		uint64 size() const {
			return internal_vector.size();
		}

		T& back() {
			return internal_vector.back();
		}

		T back() const {
			return internal_vector.back();
		}

		bool empty() const {
			return internal_vector.empty();
		}

		typename vector<T>::iterator begin() {
			return internal_vector.begin();
		}

		typename vector<T>::iterator end() {
			return internal_vector.end();
		}

		typename vector<T>::const_iterator begin() const {
			return internal_vector.begin();
		}

		typename vector<T>::const_iterator end() const {
			return internal_vector.end();
		}
	};

	template <typename K, typename V>
	struct BiMap {
		map<K, V> key_to_val;
		map<V, K> val_to_key;

		void insert(const K& key, const V& val) {
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

	template <typename T>
	struct Confirm {
		bool confirmed;
		T data;

		Confirm() {
			data = T();
			confirmed = false;
		}
		Confirm(const T& data) {
			confirmed = true;
			this->data = data;
		}
		explicit operator bool() const {
			return confirmed;
		}
	};
}
// Templates
template<typename T>
uint16 e_to_us(const T& enumerator) {
	return static_cast<uint16>(enumerator);
};

template<typename T>
T ptr(const uint64& hash) {
	return reinterpret_cast<T>(hash);
}

template <typename T>
uint64 uptr(const T uptr) {
	return reinterpret_cast<uint64>(uptr);
}

template<typename T>
uint len32(const vector<T>& vector) {
	return ul_to_u(vector.size());
};

template<typename T>
void printSize(const string& label, const vector<T>& value) {
	cout << fixed << setprecision(2) << label << " | " << static_cast<double>(sizeof(T) * value.size()) / (1024.0 * 1024.0) << " mb | " << static_cast<double>(sizeof(T) * value.size()) / 1024.0 << " kb | " << sizeof(T) * value.size() << " b" << endl;
};

template<typename T>
T f_roundToNearest(const T& num, const T& factor) {
	return round(num / factor) * factor;
};

template<typename T, typename U>
T f_roundToNearest(const T& num, const U& factor) {
	return round(num / factor) * factor;
};

template<typename T>
T f_map(const T& from_min, const T& from_max, const T& to_min, const T& to_max, const T& value) {
	return (to_min + ((to_max - to_min) / (from_max - from_min)) * (value - from_min));
}

template<typename T>
T f_mapClamped(const T& from_min, const T& from_max, const T& to_min, const T& to_max, const T& value) {
	if (value > from_max) return to_max;
	else if (value < from_min) return to_min;
	else return (to_min + ((to_max - to_min) / (from_max - from_min)) * (value - from_min));
}

template<typename T>
T f_lerp(const T& a, const T& b, const dvec1& t) {
	return a + (b - a) * t;
}

template<typename T, typename U>
T f_ramp(const map<U, T>& curve, const U& t) {
	auto lower = curve.lower_bound(t);

	if (lower == curve.begin())
		return lower->second;
	if (lower == curve.end())
		return (--lower)->second;

	const U& key_b = lower->first;
	const T& val_b = lower->second;
	--lower;
	const U& key_a = lower->first;
	const T& val_a = lower->second;

	const dvec1 t_lerp = static_cast<dvec1>(t - key_a) / static_cast<dvec1>(key_b - key_a);

	return f_lerp<T>(val_a, val_b, t_lerp);
}

template<typename K, typename V>
KL::Confirm<V> f_getMapValue(const map<K, V>& map, const K& key) {
	auto it = map.find(key);
	if (it != map.end()) {
		return  KL::Confirm(it->second);
	}
	return KL::Confirm<V>();
}

template<typename K, typename V>
bool f_hasKey(const map<K, V>& map, const K& key) {
	auto it = map.find(key);
	if (it != map.end()) {
		return true;
	}
	return false;
}

template<typename K, typename V>
KL::Confirm<V> f_getMapValue(const unordered_map<K, V>& map, const K& key) {
	auto it = map.find(key);
	if (it != map.end()) {
		return  KL::Confirm(it->second);
	}
	return  KL::Confirm<V>();
}

template<typename K, typename V>
KL::Confirm<K> f_getMapKey(const map<K, V>& map, const V& value) {
	for (const auto& pair : map) {
		if (pair.second == value) {
			return  KL::Confirm(pair.first);
		}
	}
	return  KL::Confirm<K>();
}

template<typename K, typename V>
KL::Confirm<K> f_getMapKey(const unordered_map<K, V>& map, const V& value) {
	for (const auto& pair : map) {
		if (pair.second == value) {
			return  KL::Confirm(pair.first);
		}
	}
	return  KL::Confirm<K>();
}

template<typename T>
bool f_hasVectorItem(const vector<T>& vec, const T& value) {
	auto it = find(vec.begin(), vec.end(), value);
	if (it != vec.end()) {
		return true;
	}
	return false;
}

template<typename T>
bool f_hasVectorItem(const KL::Observable_Vector<T>& vec, const T& value) {
	auto it = find(vec.begin(), vec.end(), value);
	if (it != vec.end()) {
		return true;
	}
	return false;
}

template<typename T>
KL::Confirm<uint64> f_getVectorIndex(const vector<T>& vec, const T& value) {
	auto it = find(vec.begin(), vec.end(), value);
	if (it != vec.end()) {
		return KL::Confirm(distance(vec.begin(), it));
	}
	return KL::Confirm<uint64>();
}


template<typename T>
KL::Confirm<uint64> f_getVectorIndex(const KL::Observable_Vector<T>& vec, const T& value) {
	auto it = find(vec.begin(), vec.end(), value);
	if (it != vec.end()) {
		return KL::Confirm(uint64(distance(vec.begin(), it)));
	}
	return KL::Confirm<uint64>();
}

template<typename K, typename V>
void f_removeMapItem(map<K, V>& map, const V& value) {
	for (auto it = map.begin(); it != map.end(); ) {
		if (it->second == value) {
			it = map.erase(it);
		} else {
			++it;
		}
	}
}

template<typename K, typename V>
void f_removeMapItem(unordered_map<K, V>& map, const V& value) {
	for (auto it = map.begin(); it != map.end(); ) {
		if (it->second == value) {
			it = map.erase(it);
		} else {
			++it;
		}
	}
}

template<typename K, typename V>
void f_removeMapItem(map<K, V>& map, const K& key) {
	map.erase(key);
}

template<typename K, typename V>
void f_removeMapItem(unordered_map<K, V>& map, const K& key) {
	map.erase(key);
}

template<typename T>
void f_removeVectorItem(vector<T>& vec, const T& value) {
	vec.erase(find(vec.begin(), vec.end(), value));
}

template<typename T>
void f_removeVectorItem(KL::Observable_Vector<T>& vec, const T& value) {
	vec.erase(find(vec.begin(), vec.end(), value));
}

template<typename T, typename U>
vector<T> f_flattenMap(const unordered_map<U, vector<T>>& map) {
	vector<T> flattened;
	for (const auto& pair : map) {
		const vector<T>& vec = pair.second;
		flattened.insert(flattened.end(), vec.begin(), vec.end());
	}
	return flattened;
}