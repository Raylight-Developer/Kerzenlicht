#pragma once

#include "Include.hpp"

#include "Utils/Ops.hpp"

// FWD DECL OTHER
namespace KL {
	struct Scene;
	struct Object;
	struct Transform;
	namespace SHADER {
		struct Texture;
	}
}

// FWD DECL THIS
namespace KL {
	namespace PROP {
		enum struct Type;
		enum struct Modifier;
	}
	struct Prop;
}

// DECL
namespace KL {
	namespace PROP {
		enum struct Type {
			NONE,
			ANY,
			STRING, DOUBLE, BOOL, UINT, INT,
			UMAT2, UMAT3, UMAT4, IMAT2, IMAT3, IMAT4, MAT2, MAT3, MAT4,
			UVEC2, UVEC3, UVEC4, IVEC2, IVEC3, IVEC4, VEC2, VEC3, VEC4,
			OBJECT_DATA, TRANSFORM, TEXTURE, OBJECT, SHADER, SCENE, PROP
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
		Prop(Object* data);
		Prop(SHADER::Texture* data);
		Prop(vector<dvec1> data);

		Prop operator+(const Prop& other) const;
		Prop operator-(const Prop& other) const;
		Prop operator*(const Prop& other) const;
		Prop operator/(const Prop& other) const;
		Prop pow(const Prop& other) const;

		int64  getInt()    const;
		uint64 getUint()   const;
		dvec1  getDouble() const;
		KL::Scene* getScene()    const;
		KL::Object* getObject()  const;
		KL::SHADER::Texture* getTexture()  const;

		vector<dvec1> getDoubleVector() const;


		string to_string() const;
	};
}

uvec3 typeColor(const KL::PROP::Type& type);