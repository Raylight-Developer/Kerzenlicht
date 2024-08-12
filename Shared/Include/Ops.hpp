#pragma once

#include "Include.hpp"

// string
Tokens f_split(const string& input);
Tokens f_split(const string& input, const string& delimiter);
Tokens f_closingPair(const Token_Array& tokens, const string& open = "┌", const string& close = "└");
string f_closingPair(const Tokens& lines, const string& open = "┌", const string& close = "└");
string f_join(const Tokens& tokens, const string& join, const uint64& start = 0, const uint64& end = 0);
string f_join(const Tokens& tokens, const uint64& start = 0, const uint64& end = 0);
string f_prependToLine(const string& value, const string& character);

string f_remove(const string& input, const string& remove);
string f_replace(const string& input, const string& old_str, const string& new_str);
string f_strip(const string& str);
bool   f_contains(const string& input, const string& substring);

// I/O
string loadFromFile(const string& file_path);
void writeToFile(const string& filename, const string& content);


// Opengl
string processSubShader(const string& file_path);
string preprocessShader(const string& file_path);

// Math
namespace KL {
	enum struct Rotation_Type {
		QUATERNION,
		AXIS,
		XYZ,
		XZY,
		YXZ,
		YZX,
		ZXY,
		ZYX
	};

	struct Transform {
		Rotation_Type rotation_type;
		dvec3 euler_rotation;
		dvec3 axis_rotation;
		dquat quat_rotation;
		dvec3 position;
		dvec3 scale;

		Transform(const dvec3& position = dvec3(0.0), const dvec3& rotation = dvec3(0.0), const dvec3& scale = dvec3(1.0), const Rotation_Type& type = Rotation_Type::XYZ);
		Transform(const dvec3& position, const dvec3& axis, const dvec3& rotation, const dvec3& scale, const Rotation_Type& type = Rotation_Type::AXIS);
		Transform(const dvec3& position, const dquat& rotation, const dvec3& scale, const Rotation_Type& type = Rotation_Type::QUATERNION);

		Transform operator+(const Transform& other) const;
		Transform operator-(const Transform& other) const;
		Transform operator*(const Transform& other) const;
		Transform operator/(const Transform& other) const;

		Transform operator*(const dvec1& other) const;

		void moveLocal(const dvec3& value);
		void rotate(const dvec3& value);

		dmat4 getMatrix() const;
		string to_string() const;
	};

	template<typename T>
	struct Observable_Ptr {
		T* ptr;
		map<void*, function<void()>> callbacks;

		Observable_Ptr() : ptr(nullptr) {}
		Observable_Ptr(T* pointer) : ptr(pointer) {}

		void set(T* pointer) {
			ptr = pointer;
			for (const auto& [key, func] : callbacks)
				func();
		}

		void addCallback(void* key, function<void()> func) {
			callbacks[key] = move(func);
		}

		void clearCallbacks() {
			callbacks.clear();
		}
	};
}
// Templates
template<typename T>
uint e_to_u(const T& enumerator) {
	return static_cast<uint>(enumerator);
};

template <typename T>
string ptr_to_str(const T pointer) {
	return "* " + to_string(reinterpret_cast<uint64>(pointer));
}

template <typename T>
uint64 ptr(const T pointer) {
	return reinterpret_cast<uint64>(pointer);
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
bool f_hasMapValue(const map<K, V>& map, const V& value) {
	for (const auto& pair : map) {
		if (pair.second == value) {
			return true;
		}
	}
	return false;
}

template<typename K, typename V>
bool f_hasMapValue(const unordered_map<K, V>& map, const V& value) {
	for (const auto& pair : map) {
		if (pair.second == value) {
			return true;
		}
	}
	return false;
}

template<typename K, typename V>
bool f_hasMapKey(const map<K, V>& map, const K& value) {
	for (const auto& pair : map) {
		if (pair.first == value) {
			return true;
		}
	}
	return false;
}

template<typename K, typename V>
bool f_hasMapKey(const unordered_map<K, V>& map, const K& value) {
	for (const auto& pair : map) {
		if (pair.first == value) {
			return true;
		}
	}
	return false;
}

template<typename K, typename V>
K f_getMapKey(const map<K, V>& map, const V& value) {
	for (const auto& pair : map) {
		if (pair.second == value) {
			return pair.first;
		}
	}
	throw runtime_error("Value not found in map");
}

template<typename K, typename V>
K f_getMapKey(const unordered_map<K, V>& map, const V& value) {
	for (const auto& pair : map) {
		if (pair.second == value) {
			return pair.first;
		}
	}
	throw runtime_error("Value not found in map");
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
uint64 f_getVectorIndex(const vector<T>& vec, const T& value) {
	auto it = find(vec.begin(), vec.end(), value);
	if (it != vec.end()) {
		return distance(vec.begin(), it);
	}
	return MAX_UINT64;
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