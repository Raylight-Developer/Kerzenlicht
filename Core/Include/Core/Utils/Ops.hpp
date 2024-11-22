#pragma once

#include "Core/Include.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL

// string
Tokens f_split(const string& input);
Tokens f_split(const string& input, const string& delimiter);
Tokens f_closingPair(const Token_Array& tokens, const string& open = "┌", const string& close = "└");
string f_closingPair(const Tokens& lines, const string& open = "┌", const string& close = "└");
string f_join(const Tokens& tokens, const string& join, const uint64& start = 0, const uint64& end = 0);
string f_join(const Tokens& tokens, const uint64& start = 0, const uint64& end = 0);
string f_addLinesToLine(const string& value, const string& character);

string f_remove(const string& input, const string& remove);
string f_replace(const string& input, const string& old_str, const string& new_str);
string f_strip(const string& str);
bool   f_contains(const string& input, const string& substring);

// I/O
string loadFromFile(const string& file_path);
void writeToFile(const string& file_path, const string& content);

Tokens loadFileLines(const string& file_path);
Token_Array loadFileTokens(const string& file_path);

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

	// Pitch-Yaw-Roll Order
	struct Transform {
		Rotation_Type rotation_type;
		dvec3 euler_rotation;
		dvec3 axis_rotation;
		dquat quat_rotation;
		dvec3 position;
		dvec3 scale;

		dvec3 x_vec;
		dvec3 y_vec;
		dvec3 z_vec;

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
		void orbit(const dvec3& pivot, const dvec2& py_rotation);
		void f_computeVectors();

		dmat4 getMatrix() const;
		string serialize() const;
	};
}