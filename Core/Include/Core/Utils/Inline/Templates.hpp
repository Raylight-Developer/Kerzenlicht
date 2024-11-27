#pragma once

#include "Types.hpp"

#include "Templates/List.hpp"
#include "Templates/BiMap.hpp"

#include "Templates/Observable.hpp"
#include "Templates/Observable_Map.hpp"
#include "Templates/Observable_BiMap.hpp"
#include "Templates/Observable_Vector.hpp"
#include "Templates/Observable_Pointer.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
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
uint16 e_to_u(const T& enumerator) {
	return static_cast<uint>(enumerator);
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
	cout << fixed << setprecision(2) << label << "<" << value.size() << "> | " << static_cast<double>(sizeof(T) * value.size()) / (1024.0 * 1024.0) << " mb | " << static_cast<double>(sizeof(T) * value.size()) / 1024.0 << " kb | " << sizeof(T) * value.size() << " b" << endl;
};

inline void printSizeHeader(const uint& label_len) {
	uint len = label_len > 5 ? label_len : 6;

	cout << std::setw(len) << std::left << "Label" << " │ Count       │ Mb          │ Kb            │ b               " << endl;
	for (uint i = 0; i < len - 6; i++) {
		cout << "─";
	}
	cout << "───────┼─────────────┼─────────────┼───────────────┼─────────────────" << endl;
}
template<typename T>
void printSizeRow(const string& label, const uint& label_len, const vector<T>& value) {
	cout << std::setw(label_len) << std::left << label << " │ " << std::setw(11) << std::left << value.size() << " │ " << std::setw(11) << std::left << d_to_ul(round(ul_to_d(sizeof(T) * value.size()) / (1024.0 * 1024.0))) << " │ " << std::setw(13) << std::left  << d_to_ul(round(ul_to_d(sizeof(T) * value.size()) / 1024.0)) << " │ " << std::setw(15) << std::left  << sizeof(T) * value.size() << endl;
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
bool f_hasVectorItem(const KL::Observable_List<T>& vec, const T& value) {
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
KL::Confirm<uint64> f_getVectorIndex(const KL::Observable_List<T>& vec, const T& value) {
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
void f_removeVectorItem(KL::Observable_List<T>& vec, const T& value) {
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