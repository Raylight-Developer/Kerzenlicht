#pragma once

#include "Core.hpp"

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
			STRING, DOUBLE, BOOL, UINT, INT,
			UMAT2, UMAT3, UMAT4, IMAT2, IMAT3, IMAT4, MAT2, MAT3, MAT4,
			UVEC2, UVEC3, UVEC4, IVEC2, IVEC3, IVEC4, VEC2, VEC3, VEC4,
			ANY,
			OBJECT_DATA, TRANSFORM, TEXTURE, OBJECT, SHADER, SCENE, PROP
		};
		string serialize(const Type& type);
		Type fromString(const string& type);
	}
	struct Prop : CORE::Prop {
		PROP::Type type;


		Prop(const string& data): CORE::Prop(data), type(PROP::Type::STRING) {}
		Prop(const dvec1& data) : CORE::Prop(data), type(PROP::Type::DOUBLE) {}
		Prop(const bool& data)  : CORE::Prop(data), type(PROP::Type::BOOL) {}
		Prop(const uint64& data): CORE::Prop(data), type(PROP::Type::UINT) {}
		Prop(const int64& data) : CORE::Prop(data), type(PROP::Type::INT) {}
		Prop(vector<dvec1> data): CORE::Prop(data), type(PROP::Type::DOUBLE) {}

		Prop();
		Prop(const any& data, const PROP::Type& type, const CORE::PROP::Modifier& modifier = CORE::PROP::Modifier::SINGLE);
		Prop(Object* data);
		Prop(SHADER::Texture* data);

		Prop operator+(const Prop& other) const;
		Prop operator-(const Prop& other) const;
		Prop operator*(const Prop& other) const;
		Prop operator/(const Prop& other) const;
		Prop pow(const Prop& other) const;

		KL::Scene* getScene()    const;
		KL::Object* getObject()  const;
		KL::SHADER::Texture* getTexture()  const;

		string to_string() const override;
	};
}

uvec3 typeColor(const KL::PROP::Type& type);