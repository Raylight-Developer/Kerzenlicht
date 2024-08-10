#include "Data_Property.hpp"

CLASS::Data::Data() {
	data = nullptr;
	type = DATA::Type::NONE;
	modifier = DATA::Modifier::SINGLE;
}

CLASS::Data::Data(const any& data, const DATA::Type& type, const DATA::Modifier& modifier) :
	data(data),
	type(type),
	modifier(modifier)
{}

CLASS::Data::Data(const string& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::STRING;
}
CLASS::Data::Data(const dvec1& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::DOUBLE;
}
CLASS::Data::Data(const bool& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::BOOL;
}
CLASS::Data::Data(const uint64& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::UINT;
}
CLASS::Data::Data(const int64& data) : data(data) {
	modifier = DATA::Modifier::SINGLE; type = DATA::Type::INT;
}

CLASS::Data CLASS::Data::operator+(const CLASS::Data & other) {
	if (type == other.type) {
		switch (type) {
			case DATA::Type::DOUBLE: return CLASS::Data(getDouble() + other.getDouble(), type);
		}
	}
	switch (type) {
		case DATA::Type::DOUBLE: {
			switch (other.type) {
				case DATA::Type::INT: {
					return CLASS::Data(getDouble() + other.getInt(), type);
				}
				case DATA::Type::UINT: {
					return CLASS::Data(getDouble() + other.getUint(), type);
				}
			}
		}
	}
	return CLASS::Data();
}
CLASS::Data CLASS::Data::operator-(const CLASS::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return CLASS::Data(getDouble() - other.getDouble(), type);
		}
	}
	return CLASS::Data();
}

CLASS::Data CLASS::Data::operator*(const CLASS::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return CLASS::Data(getDouble() * other.getDouble(), type);
		}
	}
	return CLASS::Data();
}

CLASS::Data CLASS::Data::operator/(const CLASS::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return CLASS::Data(getDouble() / other.getDouble(), type);
		}
	}
	return CLASS::Data();
}

int64 CLASS::Data::getInt() const {
	return any_cast<int64>(data);
}

uint64 CLASS::Data::getUint() const {
	return any_cast<uint64>(data);
}

dvec1 CLASS::Data::getDouble() const {
	switch (type) {
	case DATA::Type::DOUBLE: return any_cast<dvec1>(data);
	case DATA::Type::UINT: return static_cast<dvec1>(any_cast<uint64>(data));
	case DATA::Type::INT: return static_cast<dvec1>(any_cast<int64>(data));
	}
	return 0.0;
}

CLASS::Scene* CLASS::Data::getScene() const {
	return any_cast<CLASS::Scene*>(data);
}

CLASS::Object* CLASS::Data::getObject() const {
	return any_cast<CLASS::Object*>(data);
}

string CLASS::Data::to_string() const {
	switch (type) {
	case DATA::Type::TRANSFORM: return any_cast<Transform>(data).to_string();
	case DATA::Type::DOUBLE:    return std::to_string(any_cast<dvec1>(data));
	case DATA::Type::UINT:      return std::to_string(static_cast<dvec1>(any_cast<uint64>(data)));
	case DATA::Type::INT:       return std::to_string(static_cast<dvec1>(any_cast<int64>(data)));
	}
	return "";
}

uvec3 typeColor(const CLASS::DATA::Type& type) {
	switch (type) {
	case CLASS::DATA::Type::NONE:   return uvec3(  0,   0,   0);
	case CLASS::DATA::Type::ANY:    return uvec3(150, 150, 150);
	case CLASS::DATA::Type::STRING: return uvec3(215, 155, 135);
	case CLASS::DATA::Type::DOUBLE: return uvec3( 95, 230,  95);
	case CLASS::DATA::Type::BOOL:   return uvec3(240, 100, 175);
	case CLASS::DATA::Type::UINT:   return uvec3(105, 125,  60);
	case CLASS::DATA::Type::INT:    return uvec3( 40, 130,  40);

	case CLASS::DATA::Type::TRANSFORM: return uvec3( 85,  85, 240);
	case CLASS::DATA::Type::TEXTURE:   return uvec3(240,  85,  85);
	case CLASS::DATA::Type::OBJECT:    return uvec3(250, 175, 100);
	case CLASS::DATA::Type::SCENE:     return uvec3( 85, 195, 240);
	case CLASS::DATA::Type::DATA:      return uvec3(210, 240,  85);

	case CLASS::DATA::Type::VEC2:
	case CLASS::DATA::Type::VEC3:
	case CLASS::DATA::Type::VEC4:
	case CLASS::DATA::Type::IVEC2:
	case CLASS::DATA::Type::IVEC3:
	case CLASS::DATA::Type::IVEC4:
	case CLASS::DATA::Type::UVEC2:
	case CLASS::DATA::Type::UVEC3:
	case CLASS::DATA::Type::UVEC4: return uvec3(165, 110, 230);

	case CLASS::DATA::Type::MAT2:
	case CLASS::DATA::Type::MAT3:
	case CLASS::DATA::Type::MAT4:
	case CLASS::DATA::Type::IMAT2:
	case CLASS::DATA::Type::IMAT3:
	case CLASS::DATA::Type::IMAT4:
	case CLASS::DATA::Type::UMAT2:
	case CLASS::DATA::Type::UMAT3:
	case CLASS::DATA::Type::UMAT4: return uvec3(230, 180, 240);
	}
	return uvec3(0, 0, 0);
}

string CLASS::DATA::toString(const Type& type) {
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

CLASS::DATA::Type CLASS::DATA::fromString(const string& type) {
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