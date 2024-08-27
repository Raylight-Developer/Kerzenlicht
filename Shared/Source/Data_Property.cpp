#include "Data_Property.hpp"

#include "Object/Object.hpp"
#include "Shader/Shader.hpp"

KL::Data::Data() :
	data(nullptr),
	type(DATA::Type::NONE),
	modifier(DATA::Modifier::SINGLE)
{}

KL::Data::Data(const any& data, const DATA::Type& type, const DATA::Modifier& modifier) :
	data(data),
	type(type),
	modifier(modifier)
{}

KL::Data::Data(const string& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::STRING;
}
KL::Data::Data(const dvec1& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::DOUBLE;
}
KL::Data::Data(const bool& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::BOOL;
}
KL::Data::Data(const uint64& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::UINT;
}
KL::Data::Data(const int64& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::INT;
}

KL::Data::Data(Object* data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::OBJECT;
}

KL::Data::Data(SHADER::Texture* data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::TEXTURE;
}

KL::Data KL::Data::operator+(const KL::Data & other) {
	if (type == other.type) {
		switch (type) {
			case DATA::Type::DOUBLE: return KL::Data(getDouble() + other.getDouble(), type);
		}
	}
	switch (type) {
		case DATA::Type::DOUBLE: {
			switch (other.type) {
				case DATA::Type::INT: {
					return KL::Data(getDouble() + other.getInt(), type);
				}
				case DATA::Type::UINT: {
					return KL::Data(getDouble() + other.getUint(), type);
				}
			}
		}
	}
	return KL::Data();
}
KL::Data KL::Data::operator-(const KL::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return KL::Data(getDouble() - other.getDouble(), type);
		}
	}
	return KL::Data();
}

KL::Data KL::Data::operator*(const KL::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return KL::Data(getDouble() * other.getDouble(), type);
		}
	}
	return KL::Data();
}

KL::Data KL::Data::operator/(const KL::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return KL::Data(getDouble() / other.getDouble(), type);
		}
	}
	return KL::Data();
}

int64 KL::Data::getInt() const {
	return any_cast<int64>(data);
}

uint64 KL::Data::getUint() const {
	return any_cast<uint64>(data);
}

dvec1 KL::Data::getDouble() const {
	switch (type) {
	case DATA::Type::DOUBLE: return any_cast<dvec1>(data);
	case DATA::Type::UINT: return static_cast<dvec1>(any_cast<uint64>(data));
	case DATA::Type::INT: return static_cast<dvec1>(any_cast<int64>(data));
	}
	return 0.0;
}

KL::Scene* KL::Data::getScene() const {
	return any_cast<KL::Scene*>(data);
}

KL::Object* KL::Data::getObject() const {
	return any_cast<KL::Object*>(data);
}

KL::SHADER::Texture* KL::Data::getTexture() const {
	return any_cast<KL::SHADER::Texture*>(data);
}

string KL::Data::to_string() const {
	switch (type) {
	case DATA::Type::TRANSFORM: return any_cast<Transform>(data).toString();
	case DATA::Type::DOUBLE:    return std::to_string(any_cast<dvec1>(data));
	case DATA::Type::UINT:      return std::to_string(static_cast<dvec1>(any_cast<uint64>(data)));
	case DATA::Type::INT:       return std::to_string(static_cast<dvec1>(any_cast<int64>(data)));
	}
	return "";
}

uvec3 typeColor(const KL::DATA::Type& type) {
	switch (type) {
	case KL::DATA::Type::NONE:   return uvec3(  0,   0,   0);
	case KL::DATA::Type::ANY:    return uvec3(150, 150, 150);
	case KL::DATA::Type::STRING: return uvec3(215, 155, 135);
	case KL::DATA::Type::DOUBLE: return uvec3( 95, 230,  95);
	case KL::DATA::Type::BOOL:   return uvec3(240, 100, 175);
	case KL::DATA::Type::UINT:   return uvec3(105, 125,  60);
	case KL::DATA::Type::INT:    return uvec3( 40, 130,  40);

	case KL::DATA::Type::TRANSFORM: return uvec3( 85,  85, 240);
	case KL::DATA::Type::TEXTURE:   return uvec3(240,  85,  85);
	case KL::DATA::Type::OBJECT:    return uvec3(250, 175, 100);
	case KL::DATA::Type::SCENE:     return uvec3( 85, 195, 240);
	case KL::DATA::Type::DATA:      return uvec3(210, 240,  85);

	case KL::DATA::Type::VEC2:
	case KL::DATA::Type::VEC3:
	case KL::DATA::Type::VEC4:
	case KL::DATA::Type::IVEC2:
	case KL::DATA::Type::IVEC3:
	case KL::DATA::Type::IVEC4:
	case KL::DATA::Type::UVEC2:
	case KL::DATA::Type::UVEC3:
	case KL::DATA::Type::UVEC4: return uvec3(165, 110, 230);

	case KL::DATA::Type::MAT2:
	case KL::DATA::Type::MAT3:
	case KL::DATA::Type::MAT4:
	case KL::DATA::Type::IMAT2:
	case KL::DATA::Type::IMAT3:
	case KL::DATA::Type::IMAT4:
	case KL::DATA::Type::UMAT2:
	case KL::DATA::Type::UMAT3:
	case KL::DATA::Type::UMAT4: return uvec3(230, 180, 240);
	}
	return uvec3(0, 0, 0);
}

string KL::DATA::toString(const Type& type) {
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
		case Type::DATA:      return "DATA";
	}
	return "";
}

KL::DATA::Type KL::DATA::fromString(const string& type) {
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
	else if (type == "DATA")      return Type::DATA;
	return Type::NONE;
}