#pragma once

#include "Core/Include.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace CORE {
	namespace PROP {
		enum struct Type;
		enum struct Modifier;
	}
	struct Prop;
}

// DECL
namespace CORE {
	namespace PROP {
		enum struct Type {
			NONE,
			STRING, DOUBLE, BOOL, UINT, INT,
			UMAT2, UMAT3, UMAT4, IMAT2, IMAT3, IMAT4, MAT2, MAT3, MAT4,
			UVEC2, UVEC3, UVEC4, IVEC2, IVEC3, IVEC4, VEC2, VEC3, VEC4
		};
		enum struct Modifier {
			SINGLE,
			VECTOR,
			MAP,
			POINTER
		};
		string serialize(const Type& type);
		Type fromString(const string& type);
	}
	struct Prop {
		any data;
		PROP::Type type;
		PROP::Modifier modifier;

		Prop();
		Prop(const any& data, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE);
		Prop(const string& data);
		Prop(const dvec1& data);
		Prop(const bool& data);
		Prop(const uint64& data);
		Prop(const int64& data);
		Prop(vector<dvec1> data);

		Prop operator+(const Prop& other) const;
		Prop operator-(const Prop& other) const;
		Prop operator*(const Prop& other) const;
		Prop operator/(const Prop& other) const;
		Prop pow(const Prop& other) const;

		int64  getInt()    const;
		uint64 getUint()   const;
		dvec1  getDouble() const;
		string getStr() const;

		vector<dvec1> getDoubleVector() const;

		virtual string to_string() const;
	};
}