#pragma once

#include "Include.hpp"
#include "QT.hpp"

struct Lace;

struct S { //------------Add Space(s) to Lace------------
	uint16 count;
	S(const uint16& count = 1) : count(count) {};
	friend Lace& operator<<(Lace& lace, const S& val);
};

struct NL { //------------Add New Line(s) and/or Global Tab To Lace------------
	uint16 count;
	bool use_global_tabs;
	NL(const uint16& count = 1, const bool& use_global_tabs = true) : count(count), use_global_tabs(use_global_tabs) {}
	friend Lace& operator<<(Lace& lace, const NL& val);
};

struct TAB { //------------Add Tab(s) to Lace------------
	uint16 count;
	TAB(const uint16& count = 1) : count(count) {}
	friend Lace& operator<<(Lace& lace, const TAB& val);
};

struct CHR { //------------Add Char(s) to Lace------------
	uint16 count;
	CHR(const uint16& count = 1) : count(count) {}
	friend Lace& operator<<(Lace& lace, const CHR& val);
};

struct DEL { //------------Remove first Char(s) from Lace------------
	uint16 count;
	DEL(const uint16& count = 1) : count(count) {}
	friend Lace& operator<<(Lace& lace, const DEL& val);
};

struct POP { //------------Remove last Char(s) from Lace------------
	uint16 count;
	POP(const uint16& count = 1) : count(count) {}
	friend Lace& operator<<(Lace& lace, const POP& val);
};

struct Lace { //------------Utility for string manipulation------------
	stringstream data;
	uint16 current_tab; // Global Tabbing to be transferred through new lines
	const string character;

	Lace();
	Lace(const string& character);

	Lace& operator<< (const Lace& value);
	Lace& operator>> (const Lace& value);

	Lace& operator+= (const uint16& value);
	Lace& operator-= (const uint16& value);

	Lace& clear();
	string str() const;

	// Feed directly
	Lace& operator<< (const bool& value);
	Lace& operator<< (const char* value);
	Lace& operator<< (const string& value);
	Lace& operator<< (const stringstream& value);

	Lace& operator<< (const int8& value);
	Lace& operator<< (const int16& value);
	Lace& operator<< (const int32& value);
	Lace& operator<< (const int64& value);
	Lace& operator<< (const uint8& value);
	Lace& operator<< (const uint16& value);
	Lace& operator<< (const uint32& value);
	Lace& operator<< (const uint64& value);
	Lace& operator<< (const ivec2& value);
	Lace& operator<< (const ivec3& value);
	Lace& operator<< (const ivec4& value);
	Lace& operator<< (const uvec2& value);
	Lace& operator<< (const uvec3& value);
	Lace& operator<< (const uvec4& value);

	Lace& operator<< (const vec1& value);
	Lace& operator<< (const vec2& value);
	Lace& operator<< (const vec3& value);
	Lace& operator<< (const vec4& value);
	Lace& operator<< (const quat& value);
	Lace& operator<< (const mat2& value);
	Lace& operator<< (const mat3& value);
	Lace& operator<< (const mat4& value);

	Lace& operator<< (const dvec1& value);
	Lace& operator<< (const dvec2& value);
	Lace& operator<< (const dvec3& value);
	Lace& operator<< (const dvec4& value);
	Lace& operator<< (const dquat& value);
	Lace& operator<< (const dmat2& value);
	Lace& operator<< (const dmat3& value);
	Lace& operator<< (const dmat4& value);

	Lace& operator<< (const QPointF& value);
	Lace& operator<< (const QString& value);

	// Feed Single Units With Space Before
	Lace& operator>> (const bool& value);
	Lace& operator>> (const char* value);
	Lace& operator>> (const float& value);
	Lace& operator>> (const double& value);
	Lace& operator>> (const int8& value);
	Lace& operator>> (const int16& value);
	Lace& operator>> (const int32& value);
	Lace& operator>> (const int64& value);
	Lace& operator>> (const uint8& value);
	Lace& operator>> (const uint16& value);
	Lace& operator>> (const uint32& value);
	Lace& operator>> (const uint64& value);

	// Vectors
	Lace& operator<< (const vector<string>& value);
};