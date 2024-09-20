#include "Data_Property.hpp"

#include "Object/Object.hpp"
#include "Shader/Shader.hpp"

KL::Prop::Prop() :
	data(nullptr),
	type(PROP::Type::NONE),
	modifier(PROP::Modifier::SINGLE)
{}

KL::Prop::Prop(const any& data, const PROP::Type& type, const PROP::Modifier& modifier) :
	data(data),
	type(type),
	modifier(modifier)
{}

KL::Prop::Prop(const string& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::STRING;
}
KL::Prop::Prop(const dvec1& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::DOUBLE;
}
KL::Prop::Prop(const bool& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::BOOL;
}
KL::Prop::Prop(const uint64& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::UINT;
}
KL::Prop::Prop(const int64& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::INT;
}

KL::Prop::Prop(Object* data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::OBJECT;
}

KL::Prop::Prop(SHADER::Texture* data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::TEXTURE;
}

KL::Prop KL::Prop::operator+(const KL::Prop & other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return KL::Prop(getDouble() + other.getDouble(), type);
		}
	}
	switch (type) {
		case PROP::Type::DOUBLE: {
			switch (other.type) {
				case PROP::Type::INT: {
					return KL::Prop(getDouble() + other.getInt(), type);
				}
				case PROP::Type::UINT: {
					return KL::Prop(getDouble() + other.getUint(), type);
				}
			}
		}
	}
	return KL::Prop();
}
KL::Prop KL::Prop::operator-(const KL::Prop& other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return KL::Prop(getDouble() - other.getDouble(), type);
		}
	}
	return KL::Prop();
}

KL::Prop KL::Prop::operator*(const KL::Prop& other) const {
	if (type == other.type) {
		switch (type) {
		case PROP::Type::DOUBLE: return KL::Prop(getDouble() * other.getDouble(), type);
		}
	}
	return KL::Prop();
}

KL::Prop KL::Prop::operator/(const KL::Prop& other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return KL::Prop(getDouble() / other.getDouble(), type);
		}
	}
	return KL::Prop();
}

Prop KL::Prop::pow(const Prop& other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return KL::Prop(std::pow(getDouble(), other.getDouble()), type);
		}
	}
	return KL::Prop();
}

int64 KL::Prop::getInt() const {
	return any_cast<int64>(data);
}

uint64 KL::Prop::getUint() const {
	return any_cast<uint64>(data);
}

dvec1 KL::Prop::getDouble() const {
	switch (type) {
	case PROP::Type::DOUBLE: return any_cast<dvec1>(data);
	case PROP::Type::UINT: return static_cast<dvec1>(any_cast<uint64>(data));
	case PROP::Type::INT: return static_cast<dvec1>(any_cast<int64>(data));
	}
	return 0.0;
}

KL::Scene* KL::Prop::getScene() const {
	return any_cast<KL::Scene*>(data);
}

KL::Object* KL::Prop::getObject() const {
	return any_cast<KL::Object*>(data);
}

KL::SHADER::Texture* KL::Prop::getTexture() const {
	return any_cast<KL::SHADER::Texture*>(data);
}

string KL::Prop::to_string() const {
	switch (type) {
	case PROP::Type::TRANSFORM: return any_cast<Transform>(data).serialize();
	case PROP::Type::DOUBLE:    return std::to_string(any_cast<dvec1>(data));
	case PROP::Type::UINT:      return std::to_string(static_cast<dvec1>(any_cast<uint64>(data)));
	case PROP::Type::INT:       return std::to_string(static_cast<dvec1>(any_cast<int64>(data)));
	}
	return "";
}

uvec3 typeColor(const KL::PROP::Type& type) {
	switch (type) {
	case KL::PROP::Type::NONE:   return uvec3(  0,   0,   0);
	case KL::PROP::Type::ANY:    return uvec3(150, 150, 150);
	case KL::PROP::Type::STRING: return uvec3(215, 155, 135);
	case KL::PROP::Type::DOUBLE: return uvec3( 95, 230,  95);
	case KL::PROP::Type::BOOL:   return uvec3(240, 100, 175);
	case KL::PROP::Type::UINT:   return uvec3(105, 125,  60);
	case KL::PROP::Type::INT:    return uvec3( 40, 130,  40);

	case KL::PROP::Type::TRANSFORM: return uvec3( 85,  85, 240);
	case KL::PROP::Type::TEXTURE:   return uvec3(240,  85,  85);
	case KL::PROP::Type::OBJECT:    return uvec3(250, 175, 100);
	case KL::PROP::Type::SCENE:     return uvec3( 85, 195, 240);
	case KL::PROP::Type::PROP:      return uvec3(210, 240,  85);

	case KL::PROP::Type::VEC2:
	case KL::PROP::Type::VEC3:
	case KL::PROP::Type::VEC4:
	case KL::PROP::Type::IVEC2:
	case KL::PROP::Type::IVEC3:
	case KL::PROP::Type::IVEC4:
	case KL::PROP::Type::UVEC2:
	case KL::PROP::Type::UVEC3:
	case KL::PROP::Type::UVEC4: return uvec3(165, 110, 230);

	case KL::PROP::Type::MAT2:
	case KL::PROP::Type::MAT3:
	case KL::PROP::Type::MAT4:
	case KL::PROP::Type::IMAT2:
	case KL::PROP::Type::IMAT3:
	case KL::PROP::Type::IMAT4:
	case KL::PROP::Type::UMAT2:
	case KL::PROP::Type::UMAT3:
	case KL::PROP::Type::UMAT4: return uvec3(230, 180, 240);
	}
	return uvec3(0, 0, 0);
}

string KL::PROP::serialize(const Type& type) {
	switch (type) {
		case Type::NONE:      return "NONE";
		case Type::ANY:       return "ANY";

		case Type::STRING:    return "STRING";
		case Type::DOUBLE:    return "DOUBLE";
		case Type::BOOL:      return "BOOL";
		case Type::UINT:      return "UINT";
		case Type::INT:       return "INT";

		case Type::UMAT2:     return "UMAT2";
		case Type::UMAT3:     return "UMAT3";
		case Type::UMAT4:     return "UMAT4";
		case Type::IMAT2:     return "IMAT2";
		case Type::IMAT3:     return "IMAT3";
		case Type::IMAT4:     return "IMAT4";
		case Type::MAT2:      return "MAT2";
		case Type::MAT3:      return "MAT3";
		case Type::MAT4:      return "MAT4";

		case Type::UVEC2:     return "UVEC2";
		case Type::UVEC3:     return "UVEC3";
		case Type::UVEC4:     return "UVEC4";
		case Type::IVEC2:     return "IVEC2";
		case Type::IVEC3:     return "IVEC3";
		case Type::IVEC4:     return "IVEC4";
		case Type::VEC2:      return "VEC2";
		case Type::VEC3:      return "VEC3";
		case Type::VEC4:      return "VEC4";

		case Type::TRANSFORM: return "TRANSFORM";
		case Type::TEXTURE:   return "TEXTURE";
		case Type::OBJECT:    return "OBJECT";
		case Type::SCENE:     return "SCENE";
		case Type::PROP:      return "DATA";
	}
	return "";
}

KL::PROP::Type KL::PROP::fromString(const string& type) {
	if      (type == "NONE")      return Type::NONE;
	else if (type == "ANY")       return Type::ANY;

	else if (type == "STRING")    return Type::STRING;
	else if (type == "DOUBLE")    return Type::DOUBLE;
	else if (type == "BOOL")      return Type::BOOL;
	else if (type == "UINT")      return Type::UINT;
	else if (type == "INT")       return Type::INT;

	else if (type == "UMAT2")     return Type::UMAT2;
	else if (type == "UMAT3")     return Type::UMAT3;
	else if (type == "UMAT4")     return Type::UMAT4;
	else if (type == "IMAT2")     return Type::IMAT2;
	else if (type == "IMAT3")     return Type::IMAT3;
	else if (type == "IMAT4")     return Type::IMAT4;
	else if (type == "MAT2")      return Type::MAT2;
	else if (type == "MAT3")      return Type::MAT3;
	else if (type == "MAT4")      return Type::MAT4;

	else if (type == "UVEC2")     return Type::UVEC2;
	else if (type == "UVEC3")     return Type::UVEC3;
	else if (type == "UVEC4")     return Type::UVEC4;
	else if (type == "IVEC2")     return Type::IVEC2;
	else if (type == "IVEC3")     return Type::IVEC3;
	else if (type == "IVEC4")     return Type::IVEC4;
	else if (type == "VEC2")      return Type::VEC2;
	else if (type == "VEC3")      return Type::VEC3;
	else if (type == "VEC4")      return Type::VEC4;

	else if (type == "TRANSFORM") return Type::TRANSFORM;
	else if (type == "TEXTURE")   return Type::TEXTURE;
	else if (type == "OBJECT")    return Type::OBJECT;
	else if (type == "SCENE")     return Type::SCENE;
	else if (type == "DATA")      return Type::PROP;
	return Type::NONE;
}