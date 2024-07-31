#pragma once

#include "Include.hpp"
#include "Ops.hpp"

// FWD DECL OTHER
namespace CLASS {
	struct Scene;
	struct Object;
	struct Transform;
}

// FWD DECL THIS
namespace CLASS {
	namespace DATA {
		enum struct Type;
		enum struct Modifier;
	}
	struct Data;
}

// DECL
namespace CLASS {
	namespace DATA {
		enum struct Type {
			NONE,
			ANY,
			STRING, DOUBLE, BOOL, UINT, INT,
			MAT2, MAT3, MAT4, UMAT2, UMAT3, UMAT4, IMAT2, IMAT3, IMAT4,
			VEC2, VEC3, VEC4, UVEC2, UVEC3, UVEC4, IVEC2, IVEC3, IVEC4,
			TRANSFORM, TEXTURE, OBJECT, SCENE, DATA
		};
		enum struct Modifier {
			SINGLE,
			VECTOR,
			MAP,
			POINTER
		};
	}
	struct Data {
		any data;
		DATA::Type type;
		DATA::Modifier modifier;

		Data();
		Data(const any& data, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
		Data(const string& data);
		Data(const dvec1& data);
		Data(const bool& data);
		Data(const uint64& data);
		Data(const int64& data);

		Data operator+(const Data& other);
		Data operator-(const Data& other);
		Data operator*(const Data& other);
		Data operator/(const Data& other);

		uint64 getUint()     const;
		dvec1  getDouble()   const;
		CLASS::Scene* getScene()    const;
		CLASS::Object* getObject()  const;

		string to_string() const;
	};
}

uvec3 typeColor(const CLASS::DATA::Type& type);