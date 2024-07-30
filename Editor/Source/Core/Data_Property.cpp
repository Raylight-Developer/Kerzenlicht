#include "Core/Data_Property.hpp"

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
		case DATA::Type::DOUBLE: return CLASS::Data(any_cast<dvec1>(data) + any_cast<dvec1>(other.data), type);
		}
	}
	return CLASS::Data();
}
CLASS::Data CLASS::Data::operator-(const CLASS::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return CLASS::Data(any_cast<dvec1>(data) - any_cast<dvec1>(other.data), type);
		}
	}
	return CLASS::Data();
}

CLASS::Data CLASS::Data::operator*(const CLASS::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return CLASS::Data(any_cast<dvec1>(data) * any_cast<dvec1>(other.data), type);
		}
	}
	return CLASS::Data();
}

CLASS::Data CLASS::Data::operator/(const CLASS::Data& other) {
	if (type == other.type) {
		switch (type) {
		case DATA::Type::DOUBLE: return CLASS::Data(any_cast<dvec1>(data) / any_cast<dvec1>(other.data), type);
		}
	}
	return CLASS::Data();
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